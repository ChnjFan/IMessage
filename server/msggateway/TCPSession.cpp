//
// Created by fan on 25-4-17.
//

#include "TCPSession.h"
#include "ConnServer.h"
#include "Message.h"

std::atomic<int> TCPSession::counter(1);

TCPSession::TCPSession(tcp::socket socket, const std::shared_ptr<SessionManager> &manager)
    : tcpSocket(std::move(socket)), state(SessionState::SESSION_IDLE), sessionManager(manager), platformID(0){
    id = "ts-" + std::to_string(counter++);
}

void TCPSession::start() {
    doRead();
}

void TCPSession::stop() {
    sessionManager->removeSession(id);
    boost::system::error_code ec;
    try {
        tcpSocket.shutdown(tcp::socket::shutdown_both, ec);
        tcpSocket.close(ec);
    } catch (const boost::system::system_error &e) {
        MSG_GATEWAY_SERVER_LOG_WARN("Tcpsocket shutdown error: " + ec.message());
    }
}

void TCPSession::send(const std::string &message) {
    send(message.c_str(), message.length());
}

void TCPSession::send(const char *message, std::size_t msgSize) {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }

    const auto responseMsg = std::make_shared<Message>(message, msgSize);
    sendQueue.push(responseMsg);

    // 如果正在写操作，直接返回
    if (writeProgress)
        return;
    writeProgress = true;
    doWrite();
}

std::string TCPSession::getId() const {
    return id;
}

std::string TCPSession::getRemoteEndpoint() const {
    try {
        return tcpSocket.remote_endpoint().address().to_string();
    } catch (...) {
        return "unknown";
    }
}

bool TCPSession::extend() {
    ++trick;
    switch (state) {
        case SessionState::SESSION_INIT:
            return (trick > SESSION_HELLO_TIMEOUT);
        case SessionState::SESSION_DELETED:
            return true;
        case SessionState::SESSION_IDLE:
            return (trick > SESSION_DEFAULT_TIMEOUT);
        default:
            return true;
    }
}

void TCPSession::setState(SessionState s) {
    state = s;
}

void TCPSession::setPlatform(const int platform) {
    platformID = platform;
}


void TCPSession::setToken(const std::string &token, int64_t expires) {
    this->token = token;
    this->tokenExpire = expires;
}

SessionState TCPSession::getState() const {
    return state;
}

void TCPSession::close() {
    sessionManager->removeSession(id);
    try {
        // 客户端主动断链后连接失效，不能重复释放
        tcpSocket.shutdown(tcp::socket::shutdown_both);
        tcpSocket.close();
        setState(SessionState::SESSION_DELETED);
    } catch (...) {
        setState(SessionState::SESSION_DELETED);
    }
}

void TCPSession::readHeader() {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }
    auto self(shared_from_this());
    async_read(tcpSocket,buffer(readMsg->header(), Message::MESSAGE_DEFAULT_HEADER_SIZE),
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

void TCPSession::readBody() {
    if (SessionState::SESSION_DELETED == state) {
        return;
    }
    auto self(shared_from_this());
    async_read(tcpSocket,buffer(readMsg->body(), readMsg->getBodyLen() + Message::MESSAGE_DEFAULT_HEADER_SIZE),
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

void TCPSession::doRead() {
    readHeader();
}

void TCPSession::doWrite() {
    auto self(shared_from_this());
    if (writeMsgs.empty()) {
        return;
    }
    async_write(tcpSocket,
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
