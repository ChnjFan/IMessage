//
// Created by fan on 25-4-17.
//

#include "Session.h"
#include "ConnServer.h"
#include "Message.h"

Session::Session(const any_io_executor &ex, const std::weak_ptr<ConnServer> &server)
        : socket_(ex), state(SessionState::SESSION_IDLE), trick(0), platformID(0)
        , readMsg(std::make_shared<Message>()), connServer(server)  { }

tcp::socket & Session::socket() {
    return socket_;
}

void Session::start() {
    doRead();
}

bool Session::send(const char *buffer, std::size_t bufferSize) {
    if (SessionState::SESSION_DELETED == state) {
        return false;
    }
    const auto message = std::make_shared<Message>(buffer, bufferSize);
    writeMsgs.push_back(message);
    if (!writeMsgs.empty()) {
        doWrite();
    }
    return true;
}

bool Session::send(const std::string &buffer) {
    if (SessionState::SESSION_DELETED == state) {
        return false;
    }
    const auto message = std::make_shared<Message>(buffer.c_str(), buffer.size());
    writeMsgs.push_back(message);
    if (!writeMsgs.empty()) {
        doWrite();
    }
    return true;
}

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

void Session::setSessionInfo(const USER_SERVICE_INFO *pInfo) {
    userID = pInfo->userID;
    token = pInfo->token;
    tokenExpire = pInfo->expireTime;
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
    try {
        socket_.shutdown(tcp::socket::shutdown_both);
        socket_.close();
        setState(SessionState::SESSION_DELETED);
    } catch (...) {
        setState(SessionState::SESSION_DELETED);
    }
}

void Session::readHeader() {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }
    auto self(shared_from_this());
    async_read(socket_,buffer(readMsg->header(), Message::MESSAGE_DEFAULT_HEADER_SIZE),
        [this, self](const boost::system::error_code &ec, std::size_t length)
        {
            if (ec) {
                setState(SessionState::SESSION_DELETED);
                return;
            }
            int32_t len = 0;
            memcpy(&len, readMsg->header(), Message::MESSAGE_DEFAULT_HEADER_SIZE);
            boost::endian::big_to_native_inplace(len);
            // 消息长度超过缓冲区长度异常，与客户端断链
            if (len > readMsg->getBodySize()) {
                setState(SessionState::SESSION_DELETED);
                return;
            }
            readMsg->setBodyLength(len);
            readBody();
        });
}

void Session::readBody() {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }
    auto self(shared_from_this());
    async_read(socket_,buffer(readMsg->body(), readMsg->getBodyLen() + Message::MESSAGE_DEFAULT_HEADER_SIZE),
        [this, self](const boost::system::error_code &ec, std::size_t length)
        {
            // 消息异常关闭客户端会话，需要客户端重连
            if (ec || !readMsg->decode()) {
                setState(SessionState::SESSION_DELETED);
                return;
            }
            // TODO:处理客户端消息
            if (const auto server = connServer.lock()) {
                server->handleRequest(shared_from_this(), readMsg);
            }
            readMsg->clear();
            doRead();
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
        boost::asio::buffer(writeMsgs.front()->encode(),
            writeMsgs.front()->length()),
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
