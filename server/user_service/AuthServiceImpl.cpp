//
// Created by Fan on 25-4-28.
//

#include "AuthServiceImpl.h"
#include "UserServiceUtil.h"

AuthServiceImpl::AuthServiceImpl(const std::shared_ptr<UserServiceConfig> &config) : config(config) {
}

grpc::ServerUnaryReactor * AuthServiceImpl::getAdminToken(grpc::CallbackServerContext *context,
                                                          const user::auth::getAdminTokenReq *request, user::auth::getAdminTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(const user::auth::getAdminTokenReq *request, user::auth::getAdminTokenResp *response, const std::shared_ptr<UserServiceConfig> &config) {
            //检查 userID 是否为管理员
            if (!isAdminUserID(config, request->userid())) {
                response->set_token("");
                response->set_expiretimeseconds(0);
            } else if (!isAdminSecret(config, request->secret())) {
                response->set_token("");
                response->set_expiretimeseconds(0);
            } else {
                //TODO:设置 admin token 和过期时间
                // token 根据 userID 生成
                response->set_token("admin");
                response->set_expiretimeseconds(100000000000);
            }
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
