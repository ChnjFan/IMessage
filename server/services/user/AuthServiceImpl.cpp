//
// Created by Fan on 25-4-28.
//

#include "AuthServiceImpl.h"
#include "UserServiceUtil.h"

#define DEFAULT_ADMIN_TOKEN "admin_token_"
#define DEFAULT_ADMIN_EXPIRE_TIME   36000

AuthServiceImpl::AuthServiceImpl(const std::shared_ptr<UserServiceConfig> &config) : config(config){
}

grpc::ServerUnaryReactor * AuthServiceImpl::getAdminToken(grpc::CallbackServerContext *context,
                                                          const user::auth::getAdminTokenReq *request, user::auth::getAdminTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(const user::auth::getAdminTokenReq *request, user::auth::getAdminTokenResp *response, const std::shared_ptr<UserServiceConfig> &config) {
            //检查 userID 是否为管理员
            if (!isAdminUserID(config, request->userid())
                || !isAdminSecret(config, request->secret())) {
                Finish(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Request user is not admin"));
                return;
            }

            //TODO:设置 admin token 和过期时间
            // token 根据 userID 生成
            std::string token;
            int64_t expireTime = 0;

            response->set_token(DEFAULT_ADMIN_TOKEN + request->userid());
            response->set_expiretimeseconds(DEFAULT_ADMIN_EXPIRE_TIME);
            Finish(grpc::Status::OK);
        }

        static bool isAdminUserID(const std::shared_ptr<UserServiceConfig> &config, const std::string &userID) {
            return (config->getAdminConfig()->userID.find(userID) != config->getAdminConfig()->userID.end());
        }

        static bool isAdminSecret(const std::shared_ptr<UserServiceConfig> &config, const std::string &secret) {
            return (config->getAdminConfig()->secret == secret);
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
    return new Reactor(request, response, config);
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
