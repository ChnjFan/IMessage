//
// Created by Fan on 25-4-28.
//

#include "AuthServiceImpl.h"
#include "UserServiceUtil.h"

AuthServiceImpl::AuthServiceImpl() {
    //TODO:数据库资源初始化
}

grpc::ServerUnaryReactor * AuthServiceImpl::getAdminToken(grpc::CallbackServerContext *context,
                                                          const user::auth::getAdminTokenReq *request, user::auth::getAdminTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(const user::auth::getAdminTokenReq *request, user::auth::getAdminTokenResp *response) {
            //TODO:认证逻辑
            Finish(grpc::Status::OK);
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
    return new Reactor(request, response);
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
