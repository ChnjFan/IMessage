//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_SESSION_H
#define IMSERVER_SESSION_H

#include <list>
#include <memory>
#include <boost/asio.hpp>

#include "Message.h"

using namespace boost::asio;
using ip::tcp;

enum class SessionState {
    SESSION_INIT = 0,
    SESSION_DELETED,
    SESSION_READY,
    SESSION_IDLE,
};

class ConnServer;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(const any_io_executor& ex, const std::weak_ptr<ConnServer> &server);

    tcp::socket& socket();

    void start();

    bool send(const char* buffer, std::size_t bufferSize);

    bool extend();

    void setState(SessionState s);
    SessionState getState() const;
    std::string getUserID() const;
    std::string getPeerIP() const;

    void close();

private:
    void readHeader();
    void readBody();
    void doRead();
    void doWrite();

    const int SESSION_DEFAULT_TIMEOUT = 30;
    tcp::socket socket_;
    SessionState state;
    /* 操作时间戳，用于操作超时检测，单位秒 */
    int trick;

    /* 会话信息 */
    std::string token;
    std::string userID;
    int platformID;

    /* 消息读写缓存 */
    MessagePtr readMsg;
    std::list<MessagePtr> writeMsgs;

    std::weak_ptr<ConnServer> connServer;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif //IMSERVER_SESSION_H
