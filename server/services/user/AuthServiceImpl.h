//
// Created by Fan on 25-4-28.
//

#ifndef AUTHSERVICEIMPL_H
#define AUTHSERVICEIMPL_H

#include <grpcpp/grpcpp.h>

#include "auth/auth.grpc.pb.h"
#include "ConfigManager.h"
#include "controller/UserDatabase.h"

class AuthServiceImpl final : public user::auth::Auth::CallbackService {
public:
    explicit AuthServiceImpl(const std::shared_ptr<ConfigManager>& config);

    UserDatabase& getUserDatabase();

    grpc::ServerUnaryReactor* getUserToken(grpc::CallbackServerContext *context,
            const user::auth::getUserTokenReq *request,
            user::auth::getUserTokenResp *response) override;

    grpc::ServerUnaryReactor* parseToken(grpc::CallbackServerContext *context,
            const user::auth::parseTokenReq *request,
            user::auth::parseTokenResp *response) override;

    grpc::ServerUnaryReactor* registerUer(grpc::CallbackServerContext *context,
            const user::auth::registerUserReq *request,
            user::auth::registerUserResp *response) override;
private:
    static constexpr int64_t USER_TOKEN_EXPIRE_TIME = 86400000;
    std::shared_ptr<ConfigManager> config;
    UserDatabase userDB;
};



#endif //AUTHSERVICEIMPL_H
