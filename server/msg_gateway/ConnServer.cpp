//
// Created by fan on 25-4-13.
//

#include "server.h"
#include "ConnServer.h"

ConnServer::ConnServer(io_context &io, int port, int socketMaxConnNum,
        int socketMaxMsgLen, int socketTimeout)
    : port(port)
    , socketMaxConnNum(socketMaxConnNum)
    , socketMaxMsgLen(socketMaxMsgLen)
    , socketTimeout(socketTimeout)
    , writeBufferSize(DEFAULT_WRITE_BUFFER_SIZE)
    , onlineUserConnNum(0)
    , acceptor(io, tcp::endpoint(tcp::v4(), port))
    , taskTimer(io, chrono::seconds(1)) {
    // 每秒定时执行服务器检测任务
    taskTimer.async_wait(std::bind(&ConnServer::detectionTasks, this));
}

void ConnServer::run() {
    handler();
}

void ConnServer::changeOnlineStatus(int concurrent) {

}

void ConnServer::detectionTasks() {
    for (auto it = unauthorizedSessions.begin(); it != unauthorizedSessions.end(); ) {
        SessionPtr &session = *it;
        if (session->getState() == SessionState::SESSION_READY) {
            // 将认证过的会话建立客户端对象
            ClientPtr client = Client::constructor(session);
            clients.insert({client->getUserID(), client});
            it = unauthorizedSessions.erase(it);
        }
        else if (session->extend()) {
            // 会话建立连接后 30s 没有认证结果判定为超期，超期会话直接释放断链
            error(session, SERVER_RETURN_CODE::SERVER_RETURN_SESSION_TIMEOUT,
                std::string("Session authentication timeout, IP: " + session->getPeerIP()));
            // TODO:将 IP 记录下来进行限流
            it = unauthorizedSessions.erase(it);
        }
        else {
            ++it;
        }
    }

    taskTimer.expires_at(taskTimer.expiry() + boost::asio::chrono::seconds(1));
    taskTimer.async_wait(std::bind(&ConnServer::detectionTasks, this));
}

void ConnServer::handler() {
    SessionPtr session = std::make_shared<Session>(acceptor.get_executor());
    acceptor.async_accept(session->socket(), std::bind(
        &ConnServer::handleNewConnection, this, boost::asio::placeholders::error, session));
}

void ConnServer::handleNewConnection(const boost::system::error_code &ec, SessionPtr &session) {
    if (!ec) {
        MSG_GATEWAY_SERVER_LOG_INFO("New connection from " + session->socket().remote_endpoint().address().to_string());
        if (onlineUserConnNum >= socketMaxConnNum) {
            error(session, SERVER_RETURN_CODE::SERVER_RETURN_SESSION_OVERRUN,
                std::string("The connection exceeds the maximum limit: " + std::to_string(socketMaxConnNum)));
        }
        session->setState(SessionState::SESSION_INIT);
        unauthorizedSessions.push_back(session);
    }

    handler();
}

void ConnServer::error(SessionPtr &session, SERVER_RETURN_CODE code, std::string error) {
    MSG_GATEWAY_SERVER_LOG_ERROR("Code: " + std::to_string(static_cast<int>(code)) + ". Error: " + error);
    // TODO: 回复错误并关闭连接
    session->close();
}

