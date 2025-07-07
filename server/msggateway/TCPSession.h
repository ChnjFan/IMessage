//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_SESSION_H
#define IMSERVER_SESSION_H

#include <list>
#include <memory>
#include <boost/asio.hpp>

#include "Session.h"
#include "Message.h"
#include "UserClient.h"

using namespace boost::asio;
using ip::tcp;

enum class SessionState {
    SESSION_INIT = 0,       // 初始化状态，会话为准备好接收消息
    SESSION_DELETED,        // 会话删除，不能接收发送消息
    SESSION_READY,          // 会话已连接，未进行认证，可以接收消息，不能推送消息
    SESSION_IDLE,           // 会话空闲，可以接收或推送消息
};

class TCPSession final : public Session {
public:
    TCPSession(tcp::socket socket, const std::shared_ptr<SessionManager> &manager);

    void start() override;
    void stop() override;

    void send(const std::string& message) override;
    void send(const char* message, std::size_t msgSize) override;

    std::string getId() const override;
    std::string getRemoteEndpoint() const override;

    bool extend() override;

    void setState(SessionState s);
    void setPlatform(int platform);
    void setToken(const std::string& token, int64_t expires);

    SessionState getState() const;

    void close();

private:
    void readHeader();
    void readBody();
    void doRead();
    void doWrite();

    const int SESSION_HELLO_TIMEOUT = 30;
    const int SESSION_DEFAULT_TIMEOUT = 300;
    tcp::socket tcpSocket;
    SessionState state;

    /* 会话信息 */
    static std::atomic<int> counter;

    std::shared_ptr<SessionManager> sessionManager;
    std::string token;
    int64_t tokenExpire;
    int platformID;

    /* 消息读写缓存 */
    MessagePtr readMsg;
    std::queue<MessagePtr> sendQueue;
    bool writeProgress = false;
};

typedef std::shared_ptr<TCPSession> SessionPtr;

#endif //IMSERVER_SESSION_H
