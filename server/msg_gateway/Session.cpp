//
// Created by fan on 25-4-17.
//

#include "Session.h"

Session::Session(const boost::asio::any_io_executor &ex) :socket_(ex) {
}

boost::asio::ip::tcp::socket & Session::socket() {
    return socket_;
}
