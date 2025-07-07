//
// Created by Fan on 25-7-6.
//

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio/ip/tcp.hpp>

#include "MsgGatewayUtil.h"
#include "Session.h"
#include "TCPSession.h"

using namespace boost::asio;

class TCPServer {
public:
    TCPServer(io_context& ioc, unsigned short port, const std::shared_ptr<SessionManager> &manager)
        : acceptor(ioc, {ip::tcp::v4(), port}), socket(ioc), sessionManager(manager) {
    }

    void start() {
        doAccept();
    }
private:
    void doAccept() {
        acceptor.async_accept(socket, [this](const boost::system::error_code &ec) {
            if (ec) {
                MSG_GATEWAY_SERVER_LOG_WARN("TCP session conn error: " + ec.message());
            }
            else {
                MSG_GATEWAY_SERVER_LOG_INFO("New TCP connection from " + socket.remote_endpoint().address().to_string());
                auto session = std::make_shared<TCPSession>(socket, sessionManager);
                if (sessionManager->isExceedConnLimit()) {
                    MSG_GATEWAY_SERVER_LOG_WARN("The connection exceeds the maximum limit of a single node");
                    const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::SESSION_OVERRUN,
                    "The connection exceeds the maximum limit of a single node");
                    session->send(errInfo.c_str(), errInfo.size());
                    session->stop();
                }
                else {
                    sessionManager->addSession(session);
                    session->start();
                }
            }
            doAccept();
        });
    }

    ip::tcp::acceptor acceptor;
    ip::tcp::socket socket;
    std::shared_ptr<SessionManager> sessionManager;
};

#endif //TCPSERVER_H
