//
// Created by fan on 25-4-17.
//

#include "Session.h"
#include "ConnServer.h"
#include "Message.h"

Session::Session(const any_io_executor &ex, const std::weak_ptr<ConnServer> &server)
        : socket_(ex), state(SessionState::SESSION_IDLE), connServer(server) {
    startTime = boost::posix_time::second_clock::universal_time();
}

tcp::socket & Session::socket() {
    return socket_;
}

void Session::start() {
    // socket_.non_blocking(true); // socket 设置为非阻塞，避免线程被某个会话一直阻塞
    doRead();
}

bool Session::authen() {
    Message message;
    if (tryRead(message.body(), message.getBodySize())) {
        // TODO: 校验 token
        return true;
    }
    return false;
}

bool Session::tryRead(char* buffer, std::size_t bufferSize) {
    try {
        size_t readSize = socket_.read_some(boost::asio::buffer(buffer, bufferSize));
        if (readSize <= 0) {
            return false;
        }
    } catch (const boost::system::system_error &e) {
        return false;
    }
    return true;
}

bool Session::trySend(const char *buffer, std::size_t bufferSize) {
    Message message(buffer, bufferSize);
    writeMsgs.push_back(message);
    if (!writeMsgs.empty()) {
        doWrite();
    }
    return true;
}

//FIXME:有时会立即超时
bool Session::extend() const {
    const boost::posix_time::ptime current_time = boost::posix_time::second_clock::universal_time();
    const boost::posix_time::time_duration duration = current_time - startTime;
    if (duration.total_seconds() > SESSION_DEFAULT_TIMEOUT) {
        MSG_GATEWAY_SERVER_LOG_DEBUG("Session timeout: " + std::to_string(duration.total_seconds()));
        return true;
    }
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
    socket_.shutdown(tcp::socket::shutdown_send);
    // 设置 30s 超时确保发送完全部数据
    socket_.set_option(socket_base::linger(true, 30));
    socket_.close();
}

void Session::doRead() {
    auto self(shared_from_this());
    async_read(socket_,buffer(readMsg.body(), readMsg.getBodySize()),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec && readMsg.decode())
            {
                // TODO:处理客户端消息
                MSG_GATEWAY_SERVER_LOG_INFO(readMsg.body());
                doRead();
            }
            else
            {
                // 客户端关闭会话
                if (const auto server = connServer.lock()) {
                    MSG_GATEWAY_SERVER_LOG_INFO("Client disconnected");
                    server->deleteClient(token);
                }
            }
        });
}

void Session::doWrite() {
    auto self(shared_from_this());
    if (writeMsgs.empty()) {
        return;
    }
    async_write(socket_,
        boost::asio::buffer(writeMsgs.front().encode(),
            writeMsgs.front().length()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
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
