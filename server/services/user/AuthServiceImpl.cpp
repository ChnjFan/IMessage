//
// Created by Fan on 25-4-28.
//

#include "AuthServiceImpl.h"
#include "UserServiceUtil.h"

AuthServiceImpl::AuthServiceImpl(const std::shared_ptr<ConfigManager> &config)
    : config(config)
    , userDB(config->getDBConfig()){
}

UserDatabase& AuthServiceImpl::getUserDatabase() {
    return userDB;
}

grpc::ServerUnaryReactor * AuthServiceImpl::getUserToken(grpc::CallbackServerContext *context,
                                                         const user::auth::getUserTokenReq *request, user::auth::getUserTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(AuthServiceImpl& service, const user::auth::getUserTokenReq *request,
                user::auth::getUserTokenResp *response) {
            const UserInfo* pUserInfo = service.getUserDatabase().find(request->userid());
            if (nullptr == pUserInfo) {
                USER_SERVICE_SERVER_LOG_INFO("Login error: Not find user " + request->userid());
                Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, "Login error: Not find user"));
                return;
            }

            if (pUserInfo->getSecret() != request->secret()) {
                USER_SERVICE_SERVER_LOG_INFO("Login error: Wrong secret");
                Finish(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Login error: Wrong secret"));
                return;
            }

            //TODO:设置 admin token 和过期时间
            // token 根据 userID 生成

            Finish(grpc::Status::OK);
        }

        static bool isAdminUserID(const std::shared_ptr<ConfigManager> &config, const std::string &userID) {
            return (config->getUserServiceConfig()->adminUserID == userID);
        }

        static bool isAdminSecret(const std::shared_ptr<ConfigManager> &config, const std::string &secret) {
            return (config->getUserServiceConfig()->adminSecret == secret);
        }

    private:
        void OnDone() override {
            USER_SERVICE_SERVER_LOG_INFO("user::auth::getAdminToken Finish");
            delete this;
        }

        void OnCancel() override {
            USER_SERVICE_SERVER_LOG_INFO("user::auth::getAdminToken Cancelled");
        }
    };
    return new Reactor(*this, request, response);
}

grpc::ServerUnaryReactor * AuthServiceImpl::parseToken(grpc::CallbackServerContext *context,
    const user::auth::parseTokenReq *request, user::auth::parseTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(const user::auth::parseTokenReq *request, user::auth::parseTokenResp *response) {
            //TODO:解析 token 逻辑
            Finish(grpc::Status::OK);
        }
    private:
        void OnDone() override {
            USER_SERVICE_SERVER_LOG_INFO("user::auth::parseToken Finish");
            delete this;
        }

        void OnCancel() override {
            USER_SERVICE_SERVER_LOG_INFO("user::auth::parseToken Cancelled");
        }
    };
    return new Reactor(request, response);
}
