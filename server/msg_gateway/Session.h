//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_SESSION_H
#define IMSERVER_SESSION_H

#include <memory>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

enum class SessionState {
    SESSION_INIT = 0,
    SESSION_READY,
    SESSION_IDLE,
};

class Session {
public:
    explicit Session(const any_io_executor& ex);

    tcp::socket& socket();

    bool extend() const;

    void setState(SessionState s);
    SessionState getState() const;
    std::string getUserID() const;
    std::string getPeerIP() const;

    void close();

private:
    const int SESSION_DEFAULT_TIMEOUT = 30;
    tcp::socket socket_;
    SessionState state;
    boost::posix_time::ptime startTime;
    int timeout;
    /* 会话信息 */
    std::string token;
    std::string userID;
    int platformID;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif //IMSERVER_SESSION_H
