//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_SESSION_H
#define IMSERVER_SESSION_H

#include <list>
#include <memory>
#include <boost/asio.hpp>

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

class ConnServer;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(const any_io_executor& ex, const std::weak_ptr<ConnServer> &server);

    tcp::socket& socket();

    void start();

    bool send(const char* buffer, std::size_t bufferSize);
    bool send(const std::string& buffer);

    bool extend();

    void setState(SessionState s);
    void setPlatform(int platform);
    void setSessionInfo(const UserInfo *pInfo);
    void setToken(const std::string& token, int64_t expires);

    SessionState getState() const;
    int getUserID() const;
    std::string getPeerIP() const;

    void close();

private:
    void readHeader();
    void readBody();
    void doRead();
    void doWrite();

    const int SESSION_HELLO_TIMEOUT = 30;
    const int SESSION_DEFAULT_TIMEOUT = 300;
    tcp::socket socket_;
    SessionState state;
    /* 操作时间戳，用于操作超时检测，单位秒 */
    int trick;

    /* 会话信息 */
    std::string token;
    int userID;
    int64_t tokenExpire;
    int platformID;

    /* 消息读写缓存 */
    MessagePtr readMsg;
    std::list<MessagePtr> writeMsgs;

    std::weak_ptr<ConnServer> connServer;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif //IMSERVER_SESSION_H
