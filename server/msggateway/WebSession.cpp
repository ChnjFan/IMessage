//
// Created by Fan on 25-7-6.
//

#include "WebSession.h"
#include "MsgGatewayUtil.h"

std::atomic<int> WebSession::counter(1);

WebSession::WebSession(tcp::socket socket, const std::shared_ptr<SessionManager> &t_sessionManager)
    : websocket(std::move(socket)), sessionManager(t_sessionManager) {
    id = "ws-" + std::to_string(counter++);
}

void WebSession::start() {
    auto self = shared_from_this();
    websocket.async_accept(
        [this, self](const boost::system::error_code &ec) {
            if (ec) {
                MSG_GATEWAY_SERVER_LOG_WARN("Websocket conn error: " + ec.message());
                return;
            }

            if (sessionManager->isExceedConnLimit()) {
                MSG_GATEWAY_SERVER_LOG_WARN("Websocket exceed conn limit");
                stop();
                return;
            }

            sessionManager->addSession(shared_from_this());
            doRead();
        }
    );
}

void WebSession::stop() {
    sessionManager->removeSession(id);
    error_code ec;
    websocket.close(websocket::close_code::normal, ec);
    if (ec) {
        MSG_GATEWAY_SERVER_LOG_WARN("Websocket close error: " + ec.message());
    }
}

void WebSession::send(const std::string &message) {
    sendQueue.push(message);

    // 如果正在写操作，则直接发送
    if (writeProgress)
        return;
    writeProgress = true;
    doWrite();
}

void WebSession::send(const char *message, const std::size_t msgSize) {
    send(std::string(message, msgSize));
}

std::string WebSession::getId() const {
    return id;
}

std::string WebSession::getRemoteEndpoint() const {
    try {
        return websocket.next_layer().socket().remote_endpoint().address().to_string();
    } catch (...) {
        return "unknown";
    }
}

bool WebSession::extend() {
    return (++trick >= 30);
}

void WebSession::doRead() {
    auto self = shared_from_this();
    websocket.async_read(buffer, [this, self](const error_code &ec, std::size_t bytes_transferred) {
        // 忽略参数 bytes_transferred，可以不携带改该参数
        boost::ignore_unused(bytes_transferred);
        if (ec == websocket::error::closed) {
            MSG_GATEWAY_SERVER_LOG_INFO("Websocket: " + getRemoteEndpoint() + " closed.");
        }

        if (ec) {
            MSG_GATEWAY_SERVER_LOG_WARN("Websocket read error: " + ec.message());
            return;
        }

        if (messageCallback_) {
            const std::string message = buffers_to_string(buffer.data());
            messageCallback_(shared_from_this(), message);
        }

        // 继续读取下条消息
        buffer.consume(buffer.size());
        doRead();
    });
}

void WebSession::doWrite() {
    auto self = shared_from_this();
    websocket.async_write(net::buffer(sendQueue.front()), [this, self](const error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            MSG_GATEWAY_SERVER_LOG_WARN("Websocket write error: " + ec.message());
            return;
        }

        // 已发送队列出队，未发送完继续发送
        sendQueue.pop();
        if (!sendQueue.empty()) {
            doWrite();
        }
        else {
            writeProgress = false;
        }
    });
}

