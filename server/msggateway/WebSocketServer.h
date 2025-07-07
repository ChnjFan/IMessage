//
// Created by Fan on 25-7-6.
//

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>

#include "Session.h"
#include "WebSession.h"

using namespace boost::asio;

class WebSocketServer {
public:
    WebSocketServer(io_context& context, unsigned short port, const std::shared_ptr<SessionManager>& manager)
        : acceptor(context, {ip::tcp::v4(), port}), socket(context), sessionManager(manager) {}

    void start() {
        doAccept();
    }

private:
    void doAccept() {
        acceptor.async_accept(socket,
            [this](const boost::beast::error_code& ec) {
                if (!ec) {
                    // 创建新的WebSocket会话并启动
                    std::make_shared<WebSession>(std::move(socket), sessionManager)->start();
                }

                // 继续接受下一个连接
                doAccept();
            });
    }

    ip::tcp::acceptor acceptor;
    ip::tcp::socket socket;
    std::shared_ptr<SessionManager> sessionManager;
};

#endif //WEBSERVER_H
