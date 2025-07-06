//
// Created by Fan on 25-7-5.
//

#ifndef SESSION_H
#define SESSION_H

#include <memory>
#include <functional>
#include <map>

#include "Message.h"
#include "MsgGatewayUtil.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    virtual ~Session() = default;

    // 会话生命周期管理
    virtual void start() = 0;
    virtual void stop() = 0;

    // 数据发送
    virtual void send(const std::string& message) = 0;
    virtual void send(const char* message, std::size_t msgSize) = 0;

    // 会话信息
    virtual std::string getId() const = 0;
    virtual std::string getRemoteEndpoint() const = 0;

    virtual bool extend() = 0;

    // 设置回调函数
    void setMessageCallback(const std::function<void(std::shared_ptr<Session>, const std::string&)> &callback) {
        messageCallback_ = callback;
    }

    void setErrorCallback(const std::function<void(std::shared_ptr<Session>, const std::string&)> &callback) {
        errorCallback_ = callback;
    }

    void setCloseCallback(const std::function<void(std::shared_ptr<Session>)> &callback) {
        closeCallback_ = callback;
    }

protected:
    std::string id;     // 会话 ID
    int trick = 0;      // 操作时间戳，用于操作超时检测，单位秒
    std::function<void(std::shared_ptr<Session>, const std::string&)> messageCallback_;
    std::function<void(std::shared_ptr<Session>, const std::string&)> errorCallback_;
    std::function<void(std::shared_ptr<Session>)> closeCallback_;
};

class SessionManager {
public:
    void addSession(const std::shared_ptr<Session> &session) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_[session->getId()] = session;

        // 设置会话关闭回调
        session->setCloseCallback([this](const std::shared_ptr<Session> &s) {
            removeSession(s->getId());
        });
    }

    void removeSession(const std::string& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.erase(id);
    }

    void sendToAll(const std::string& message) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto&[id, session] : sessions_) {
            session->send(message);
        }
    }

    std::size_t getSessionCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sessions_.size();
    }

    bool isExceedConnLimit() const {
        // TODO 后续增加配置
        return (sessions_.size() >= 1000);
    }

    void check() {
        // todo 考虑怎么能检测所有会话
        for (const auto&[id, session] : sessions_) {
            if (session->extend()) {
                MSG_GATEWAY_SERVER_LOG_ERROR("Session Timeout. IP: " + session->getRemoteEndpoint());
                const std::string errInfo = Message::responseFormat(SERVER_RETURN_CODE::SESSION_TIMEOUT,
                    "Session send hello timeout.");
                session->send(errInfo);
                session->stop();
                break;
            }
        }
    }

private:
    mutable std::mutex mutex_;
    std::map<std::string, std::shared_ptr<Session>> sessions_;
};

#endif //SESSION_H
