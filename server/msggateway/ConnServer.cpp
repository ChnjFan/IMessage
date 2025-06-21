//
// Created by fan on 25-4-13.
//

#include <boost/json.hpp>
#include "ConnServer.h"
#include "Returncode.h"

#include "UserClient.h"
#include "Message.h"

ConnServer::ConnServer(io_context &io, std::shared_ptr<ConfigManager> &configMgr, const int port, const int socketMaxConnNum,
                       const int socketMaxMsgLen, const int socketTimeout)
      : port(port)
      , socketMaxConnNum(socketMaxConnNum)
      , socketMaxMsgLen(socketMaxMsgLen)
      , socketTimeout(socketTimeout)
      , writeBufferSize(DEFAULT_WRITE_BUFFER_SIZE)
      , onlineSessionNum(0)
      , acceptor(io, tcp::endpoint(tcp::v4(), port))
      , taskTimer(io, chrono::seconds(1)) {
    const std::string userServiceTarget = configMgr->getUserServiceConfig()->serviceConfig.registerIP + ":"
                                    + std::to_string(configMgr->getUserServiceConfig()->serviceConfig.ports[0]);
    clientManager.registerService(SERVICE_NAME::SERVICE_USER, userServiceTarget);
    // 每秒定时执行服务器检测任务
    startDetectionLoop();
}

void ConnServer::run() {
    auto session = std::make_shared<Session>(acceptor.get_executor(), shared_from_this());
    acceptor.async_accept(session->socket(), [this, session](const boost::system::error_code &errCode) {
        if (errCode) {
            MSG_GATEWAY_SERVER_LOG_WARN("Client conn error: " + errCode.message());
        }
        else {
            MSG_GATEWAY_SERVER_LOG_INFO("New connection from " + session->socket().remote_endpoint().address().to_string());
            // 超过单个节点的连接限制
            if (onlineSessionNum >= socketMaxConnNum) {
                MSG_GATEWAY_SERVER_LOG_WARN("The connection exceeds the maximum limit of a single node: "
                    + std::to_string(socketMaxConnNum));
                const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::SESSION_OVERRUN,
                    "The connection exceeds the maximum limit of a single node");
                session->send(errInfo.c_str(), errInfo.size());
                session->close();
                return;
            }

            std::lock_guard lock(mutex);
            // 新建链接状态 INIT 进入未认证队列，超过 5 分钟未登录主动断链
            session->setState(SessionState::SESSION_INIT);
            unauthorizedSessions.push_back(session);
            ++onlineSessionNum;
            session->start();
        }
        run();
    });
}

void ConnServer::handleRequest(const SessionPtr& session, const MessagePtr &message) {
    switch (session->getState()) {
        case SessionState::SESSION_INIT:
            helloRequest(session, message);
            break;
        case SessionState::SESSION_READY:
            newSessionRequest(session, message);
            break;
        default:
            break;
    }
}

void ConnServer::startDetectionLoop() {
    taskTimer.expires_after(chrono::seconds(1));
    taskTimer.async_wait([this](const boost::system::error_code &errCode) {
        if (errCode) {
            MSG_GATEWAY_SERVER_LOG_ERROR("Timer error: " + errCode.message());
            return;
        }
        unauthSessionDetector();
        startDetectionLoop();
    });
}

void ConnServer::unauthSessionDetector() {
    for (auto it = unauthorizedSessions.begin(); it != unauthorizedSessions.end(); ) {
        if (const SessionPtr &session = *it; session->getState() >= SessionState::SESSION_IDLE) {
            std::lock_guard lock(mutex);
            it = unauthorizedSessions.erase(it);
        }
        else if (session->getState() == SessionState::SESSION_DELETED) {
            std::lock_guard lock(mutex);
            it = unauthorizedSessions.erase(it);
            --onlineSessionNum;
        }
        else if (session->extend()) {
            // TODO 设置心跳包 30s 超时
            // 会话建立连接 30s 没有发送 hello 报文判定为超期，超期会话回复客户端超期后直接释放断链，此时客户端需要重新建链
            MSG_GATEWAY_SERVER_LOG_WARN("Session send hello timeout, IP: " + session->getPeerIP());
            const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::SESSION_TIMEOUT,
                    "Session send hello timeout.");
            session->send(errInfo.c_str(), errInfo.size());
            session->close();
            // TODO:将 IP 记录下来进行限流
            std::lock_guard lock(mutex);
            it = unauthorizedSessions.erase(it);
            --onlineSessionNum;
        }
        else {
            ++it;
        }
    }
}

/**
 * @brief hello 报文处理
 * @param session 会话终端
 * @param message 请求消息
 * @note 客户端成功建链后立即发送 hello 报文，会话状态转换为 READY 可以接收客户端登录注册消息
 */
void ConnServer::helloRequest(const SessionPtr &session, const MessagePtr &message) {
    if (message->getMethod() != MESSAGE_REQUEST_HELLO) {
        MSG_GATEWAY_SERVER_LOG_WARN("Session state: INIT. Message request: " + message->getMethod());
        const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
            "Client request message error");
        session->send(errInfo.c_str(), errInfo.size());
        return;
    }

    boost::json::value json = boost::json::parse(message->getMessage());

    session->setState(SessionState::SESSION_READY);
    session->setPlatform(json.at("platform").as_int64());

    const std::string res = Message::responseFormat(SERVER_RETURN_CODE::SUCCESS, "Hello success");
    session->send(res.c_str(), res.size());
}

/**
 * @brief 新会话请求消息处理
 * @param session 会话终端
 * @param message 请求消息
 * @note 新会话请求消息只能是终端用户认证或者用户注册消息
 */
void ConnServer::newSessionRequest(const SessionPtr &session, const MessagePtr &message) {
    UserClient userClient(clientManager.getChannel(SERVICE_NAME::SERVICE_USER));

    if (message->getMethod() == MESSAGE_REQUEST_REGISTER) {
        USER_SERVICE_INFO *pUserInfo = UserClient::parseRegisterRequest(message->getMessage());
        if (nullptr == pUserInfo) {
            MSG_GATEWAY_SERVER_LOG_WARN("User client register message error");
            const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
                "Register request error");
            session->send(errInfo.c_str(), errInfo.size());
        }
    }
    else if (message->getMethod() != MESSAGE_REQUEST_AUTH) {
        USER_SERVICE_INFO *pUserInfo = UserClient::parseLoginRequest(message->getMessage());
        if (nullptr == pUserInfo) {
            MSG_GATEWAY_SERVER_LOG_WARN("User client login message error");
            const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
                "Login request error");
            session->send(errInfo.c_str(), errInfo.size());
            return;
        }
        if (UserClient::isAdminID(pUserInfo->userID)) {
            if (userClient.getAdminToken(pUserInfo->userID, pUserInfo->secret, &pUserInfo->token, &pUserInfo->expireTime)) {
                session->setSessionInfo(pUserInfo);
                session->setState(SessionState::SESSION_IDLE);

                ClientPtr client = Client::constructor(session);
                client->setType(CLIENT_TYPE::ADMIN);
                clients.insert({client->getUserID(), client});
                --onlineSessionNum;
                MSG_GATEWAY_SERVER_LOG_DEBUG("User online, current node client num: " + std::to_string(clients.size()));
            }
            else {
                MSG_GATEWAY_SERVER_LOG_WARN("Client get admin token error, IP: " + session->getPeerIP());
                const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::AUTH_ERROR, message->getMessage());
                session->send(errInfo.c_str(), errInfo.size());
            }
        }
        else {

        }
        delete pUserInfo;
    }
    else {
        // 新会话的请求只能是注册消息或认证消息
        MSG_GATEWAY_SERVER_LOG_WARN("Session state: READY. Message request: " + message->getMethod());
        const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
            "Client request message error");
        session->send(errInfo.c_str(), errInfo.size());
    }
}
