//
// Created by Fan on 25-7-6.
//

#ifndef WEBSESSION_H
#define WEBSESSION_H

#include <queue>

#include "Session.h"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast.hpp"

using namespace boost::asio::ip;
using namespace boost::beast;

class WebSession final : public Session {
public:
    WebSession(tcp::socket socket, const std::shared_ptr<SessionManager> &t_sessionManager);

    void start() override;
    void stop() override;

    void send(const std::string &message) override;
    void send(const char *message, std::size_t msgSize) override;

    std::string getId() const override;
    std::string getRemoteEndpoint() const override;

    bool extend() override;

private:
    void onAccept(const error_code &ec);
    void doRead();
    void doWrite();

    websocket::stream<tcp_stream> websocket;
    flat_buffer buffer;
    std::shared_ptr<SessionManager> sessionManager;
    static std::atomic<int> counter;
    bool writeProgress = false;
    std::queue<std::string> sendQueue; // 发送消息队列
};



#endif //WEBSESSION_H
