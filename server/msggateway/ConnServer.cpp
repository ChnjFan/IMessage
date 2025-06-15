//
// Created by fan on 25-4-13.
//

#include <boost/json.hpp>
#include "ConnServer.h"

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
    std::string userServiceTarget = configMgr->getUserServiceConfig()->serviceConfig.registerIP + ":"
                                    + std::to_string(configMgr->getUserServiceConfig()->serviceConfig.ports[0]);
    clientManager.registerService(SERVICE_NAME::SERVICE_USER, userServiceTarget);
    // 每秒定时执行服务器检测任务
    taskTimer.async_wait(std::bind(&ConnServer::detectionTasks, this));
}

void ConnServer::run() {
    handler();
}

void ConnServer::handleRequest(const SessionPtr& session, MessagePtr &message) {
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

void ConnServer::deleteClient(const std::string &token) {
    auto it = clients.find(token);
    if (it != clients.end()) {
        it->second->close();
    }
}

void ConnServer::detectionTasks() {
    handleUnauthSession();
    handleClients();

    taskTimer.expires_at(taskTimer.expiry() + boost::asio::chrono::seconds(1));
    taskTimer.async_wait(std::bind(&ConnServer::detectionTasks, this));
}

void ConnServer::handleUnauthSession() {
    for (auto it = unauthorizedSessions.begin(); it != unauthorizedSessions.end(); ) {
        SessionPtr &session = *it;
        if (session->getState() >= SessionState::SESSION_IDLE) {
            it = unauthorizedSessions.erase(it);
        }
        else if (session->getState() == SessionState::SESSION_DELETED) {
            it = unauthorizedSessions.erase(it);
            --onlineSessionNum;
        }
        else if (session->extend()) {
            // 会话建立连接 30s 没有认证结果判定为超期，超期会话回复客户端超期后直接释放断链，此时客户端需要重新建链
            error(session, SERVER_RETURN_CODE::SERVER_RETURN_SESSION_AUTH_TIMEOUT,
                std::string("Session authentication timeout, IP: " + session->getPeerIP()));
            // TODO:将 IP 记录下来进行限流
            it = unauthorizedSessions.erase(it);
            --onlineSessionNum;
        }
        else {
            ++it;
        }
    }
}

void ConnServer::handleClients() {
    for (auto it = clients.begin(); it != clients.end(); ) {
        if (const ClientPtr client = it->second; client->down()) {
            it = clients.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ConnServer::handler() {
    SessionPtr session = std::make_shared<Session>(acceptor.get_executor(), shared_from_this());
    acceptor.async_accept(session->socket(), [this, session](const boost::system::error_code &errCode) {
        if (errCode) {
            MSG_GATEWAY_SERVER_LOG_WARN("Client conn error: " + errCode.message());
        }
        else {
            MSG_GATEWAY_SERVER_LOG_INFO("New connection from " + session->socket().remote_endpoint().address().to_string());
            if (onlineSessionNum >= socketMaxConnNum) {
                error(session, SERVER_RETURN_CODE::SERVER_RETURN_SESSION_OVERRUN,
                      std::string("The connection exceeds the maximum limit: " + std::to_string(socketMaxConnNum)));
            }
            session->setState(SessionState::SESSION_INIT);
            unauthorizedSessions.push_back(session);
            ++onlineSessionNum;
            session->start();
        }

        handler();
    });
}

std::string ConnServer::revertTokenToJson(const std::string &token) {
    boost::json::object res;
    res["token"] = token;
    std::string tokenStr = boost::json::serialize(res);
    MSG_GATEWAY_SERVER_LOG_DEBUG("IM server response: " + tokenStr);
    return tokenStr;
}

/**
 * @brief hello 报文处理
 * @param session 会话终端
 * @param message 请求消息
 * @note 客户端成功建链后立即发送 hello 报文，会话状态转换为 READY 可以接收客户端登录注册消息
 */
void ConnServer::helloRequest(const SessionPtr &session, const MessagePtr &message) {
    if (message->getMethod() != MESSAGE_REQUEST_HELLO) {
        error(session, SERVER_RETURN_CODE::CLIENT_RETURN_REQUEST_ERROR, std::string("Client request error"));
        return;
    }
    session->setState(SessionState::SESSION_READY);
}

/**
 * @brief 新会话请求消息处理
 * @param session 会话终端
 * @param message 请求消息
 * @note 新会话请求消息只能是终端用户认证或者用户注册消息
 */
void ConnServer::newSessionRequest(const SessionPtr &session, const MessagePtr &message) {
    // 新会话的请求只能是注册消息或认证消息
    if (message->getMethod() != MESSAGE_REQUEST_REGISTER && message->getMethod() != MESSAGE_REQUEST_AUTH) {
        error(session, SERVER_RETURN_CODE::CLIENT_RETURN_REQUEST_ERROR, std::string("Client request error"));
        return;
    }

    UserClient userClient(clientManager.getChannel(SERVICE_NAME::SERVICE_USER));
    USER_SERVICE_INFO *pUserInfo = UserClient::parseLoginRequest(message->body() + message->getMethod().length()+1);

    if (userClient.getAdminToken(pUserInfo->userID, pUserInfo->secret, &pUserInfo->token, &pUserInfo->expireTime)) {
        session->setSessionInfo(pUserInfo);
        session->setState(SessionState::SESSION_IDLE);

        ClientPtr client = Client::constructor(session);
        client->setType(CLIENT_TYPE::ADMIN);
        clients.insert({client->getUserID(), client});
        --onlineSessionNum;
        MSG_GATEWAY_SERVER_LOG_DEBUG("User online, current node client num: " + std::to_string(clients.size()));

        // TODO:回复建链消息
        const std::string res = revertTokenToJson(pUserInfo->token);
        session->send(res);
    }
    else {
        error(session, SERVER_RETURN_CODE::CLIENT_RETURN_REQUEST_ERROR, "Authentication failed");
    }

    delete pUserInfo;
}

void ConnServer::error(const SessionPtr &session, SERVER_RETURN_CODE code, std::string error) {
    MSG_GATEWAY_SERVER_LOG_WARN("Code: " + std::to_string(static_cast<int>(code)) + ". Error: " + error);
    // TODO: 回复错误并关闭连接
    session->send(error.c_str(), error.size());
    session->close();
}

