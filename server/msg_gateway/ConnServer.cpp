//
// Created by fan on 25-4-13.
//

#include "server.h"
#include "ConnServer.h"

ConnServer::ConnServer(boost::asio::io_context &io, int port, int socketMaxConnNum,
        int socketMaxMsgLen, int socketTimeout)
    : port(port)
    , socketMaxConnNum(socketMaxConnNum)
    , socketMaxMsgLen(socketMaxMsgLen)
    , socketTimeout(socketTimeout)
    , writeBufferSize(DEFAULT_WRITE_BUFFER_SIZE)
    , onlineUserConnNum(0)
    , acceptor(io, tcp::endpoint(tcp::v4(), port)) {

}

void ConnServer::run() {
    handler();
}

void ConnServer::changeOnlineStatus(int concurrent) {

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
            // TODO: 回复错误并关闭连接
            return;
        }
    }

    handler();
}
