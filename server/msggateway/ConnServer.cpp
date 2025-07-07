//
// Created by fan on 25-4-13.
//

#include <boost/json.hpp>
#include "ConnServer.h"
#include "Returncode.h"

#include "UserClient.h"
#include "Message.h"
#include "TCPServer.h"
#include "WebSocketServer.h"
#include "model/UserInfo.h"

ConnServer::ConnServer(io_context &context, const std::shared_ptr<ConfigManager> &configMgr, const int port, const int socketMaxConnNum,
                       const int socketMaxMsgLen, const int socketTimeout)
      : tcpPort(port)
      , socketMaxConnNum(socketMaxConnNum)
      , socketMaxMsgLen(socketMaxMsgLen)
      , socketTimeout(socketTimeout)
      , writeBufferSize(DEFAULT_WRITE_BUFFER_SIZE)
      , taskTimer(context, chrono::seconds(1)) {
    const std::string userServiceTarget = configMgr->getUserServiceConfig()->serviceConfig.registerIP + ":"
                                    + std::to_string(configMgr->getUserServiceConfig()->serviceConfig.ports[0]);
    clientManager.registerService(SERVICE_NAME::SERVICE_USER, userServiceTarget);
    // 每秒定时执行服务器检测任务
    startDetectionLoop();
}

void ConnServer::run(io_context &context) {
    TCPServer tcpServer(context, tcpPort, std::shared_ptr<SessionManager>(&sessionManager));
    tcpServer.start();
    WebSocketServer webSocketServer(context, 8080, std::shared_ptr<SessionManager>(&sessionManager));
    webSocketServer.start();
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
        sessionManager.check();
        startDetectionLoop();
    });
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
    session->setPlatform(static_cast<int>(json.at("platform").as_int64()));

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
    const std::string requestMethod = message->getMethod();
    if (requestMethod == MESSAGE_REQUEST_REGISTER) {
        return registerRequest(session, message);
    }

    if (requestMethod == MESSAGE_REQUEST_AUTH) {
        return loginRequest(session, message);
    }

    // 新会话的请求只能是注册消息或认证消息
    MSG_GATEWAY_SERVER_LOG_WARN("Session state: READY. Message request: " + message->getMethod());
    const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
        "Client request message error");
    session->send(errInfo.c_str(), errInfo.size());
}

void ConnServer::registerRequest(const SessionPtr &session, const MessagePtr &message) {
    const UserClient userClient(clientManager.getChannel(SERVICE_NAME::SERVICE_USER));
    const UserInfo *pUserInfo = UserClient::parseRegisterRequest(message->getMessage());
    if (nullptr == pUserInfo) {
        MSG_GATEWAY_SERVER_LOG_WARN("User client register message error");
        const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
            "Register request error");
        session->send(errInfo.c_str(), errInfo.size());
    }

    if (userClient.registerUser(pUserInfo)) {
        const std::string registerResponse = Message::responseFormat(SERVER_RETURN_CODE::SUCCESS,
                        "Register success");
        session->send(registerResponse.c_str(), registerResponse.size());
    }
    else {
        const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REGISTER_ERROR,
                                    "Register fail");
        session->send(errInfo.c_str(), errInfo.size());
    }
    delete pUserInfo;
}

void ConnServer::loginRequest(const SessionPtr &session, const MessagePtr &message) {
    const UserClient userClient(clientManager.getChannel(SERVICE_NAME::SERVICE_USER));
    const UserInfo *pUserInfo = UserClient::parseLoginRequest(message->getMessage());
    if (nullptr == pUserInfo) {
        MSG_GATEWAY_SERVER_LOG_WARN("User client login message error");
        const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::REQUEST_ERROR,
            "Login request error");
        session->send(errInfo.c_str(), errInfo.size());
        return;
    }

    std::string token;
    int64_t expires = 0;
    if (userClient.getUserToken(pUserInfo, &token, &expires)) {
        session->setState(SessionState::SESSION_IDLE);
        session->setToken(token, expires);
    }
    else {
        MSG_GATEWAY_SERVER_LOG_WARN("Client get admin token error, IP: " + session->getRemoteEndpoint());
        const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::AUTH_ERROR, message->getMessage());
        session->send(errInfo.c_str(), errInfo.size());
    }

    delete pUserInfo;
}

