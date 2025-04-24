//
// Created by fan on 25-4-17.
//

#include "Session.h"
#include "ConnServer.h"
#include "Message.h"

Session::Session(const any_io_executor &ex, const std::weak_ptr<ConnServer> &server)
        : socket_(ex), state(SessionState::SESSION_IDLE), trick(0), platformID(0), connServer(server)  { }

tcp::socket & Session::socket() {
    return socket_;
}

void Session::start() {
    // socket_.non_blocking(true); // socket 设置为非阻塞，避免线程被某个会话一直阻塞
    doRead();
}

bool Session::send(const char *buffer, std::size_t bufferSize) {
    if (SessionState::SESSION_DELETED == state) {
        return false;
    }
    Message message(buffer, bufferSize);
    writeMsgs.push_back(message);
    if (!writeMsgs.empty()) {
        doWrite();
    }
    return true;
}

//FIXME:有时会立即超时
bool Session::extend() {
    if (trick > SESSION_DEFAULT_TIMEOUT) {
        return true;
    }
    ++trick;
    return false;
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
    socket_.shutdown(tcp::socket::shutdown_both);
    socket_.close();
    setState(SessionState::SESSION_DELETED);
}

void Session::readHeader() {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }
    auto self(shared_from_this());
    async_read(socket_,buffer(readMsg.header(), Message::MESSAGE_DEFAULT_HEADER_SIZE),
        [this, self](const boost::system::error_code &ec, std::size_t length)
        {
            if (!ec)
            {
                int len = 0;
                memcpy(&len, readMsg.body(), Message::MESSAGE_DEFAULT_HEADER_SIZE);
                boost::endian::big_to_native_inplace(len);
                readMsg.setBodyLength(len);
                readBody();
            }
            else
            {
                // 客户端关闭会话
                setState(SessionState::SESSION_DELETED);
            }
        });
}

void Session::readBody() {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }
    auto self(shared_from_this());
    async_read(socket_,buffer(readMsg.body(), readMsg.getBodyLen()),
        [this, self](const boost::system::error_code &ec, std::size_t length)
        {
            // 消息异常关闭客户端会话，需要客户端重连
            if (!ec && readMsg.decode())
            {
                // TODO:处理客户端消息
                MSG_GATEWAY_SERVER_LOG_INFO(readMsg.body());
                doRead();
            }
            else
            {
                // 客户端关闭会话
                setState(SessionState::SESSION_DELETED);
            }
        });
}

void Session::doRead() {
    readHeader();
}

void Session::doWrite() {
    auto self(shared_from_this());
    if (writeMsgs.empty()) {
        return;
    }
    async_write(socket_,
        boost::asio::buffer(writeMsgs.front().encode(),
            writeMsgs.front().length()),
            [this, self](const boost::system::error_code &ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    writeMsgs.pop_front();
                    if (!writeMsgs.empty())
                    {
                        doWrite();
                    }
                }
            });
}
