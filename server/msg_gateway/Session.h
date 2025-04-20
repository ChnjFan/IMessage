//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_SESSION_H
#define IMSERVER_SESSION_H

#include <memory>
#include <boost/asio.hpp>

class Session {
public:
    Session() = default;
    Session(const Session&) = default;
    explicit Session(const boost::asio::any_io_executor& ex);

    boost::asio::ip::tcp::socket& socket();

private:
    boost::asio::ip::tcp::socket socket_;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif //IMSERVER_SESSION_H
