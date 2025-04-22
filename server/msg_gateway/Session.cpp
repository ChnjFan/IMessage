//
// Created by fan on 25-4-17.
//

#include "Session.h"

Session::Session(const any_io_executor &ex) :socket_(ex), state(SessionState::SESSION_IDLE), timeout(SESSION_DEFAULT_TIMEOUT) {
    startTime = boost::posix_time::second_clock::universal_time();
}

tcp::socket & Session::socket() {
    return socket_;
}

bool Session::extend() const {
    const boost::posix_time::ptime current_time = boost::posix_time::second_clock::universal_time();
    const boost::posix_time::time_duration duration = current_time - startTime;
    return duration.total_seconds() > timeout;
}

void Session::setState(SessionState s) {
    state = s;
}

SessionState Session::getState() const {
    return state;
}

std::string Session::getUserID() const {
    return userID;
}

std::string Session::getPeerIP() const {
    return socket_.remote_endpoint().address().to_string();
}

void Session::close() {
    socket_.shutdown(tcp::socket::shutdown_send);
    // 设置 30s 超时确保发送完全部数据
    socket_.set_option(socket_base::linger(true, 30));
    socket_.close();
}
