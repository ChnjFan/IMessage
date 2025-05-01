//
// Created by Fan on 25-4-28.
//

#ifndef AUTHSERVICEIMPL_H
#define AUTHSERVICEIMPL_H

#include <grpcpp/grpcpp.h>

#include "auth/auth.grpc.pb.h"
#include "UserServiceConfig.h"

class AuthServiceImpl final : public user::auth::Auth::CallbackService {
public:
    explicit AuthServiceImpl(const std::shared_ptr<UserServiceConfig>& config);

    grpc::ServerUnaryReactor* getAdminToken(grpc::CallbackServerContext *context,
            const user::auth::getAdminTokenReq *request,
            user::auth::getAdminTokenResp *response) override;

    grpc::ServerUnaryReactor* parseToken(grpc::CallbackServerContext *context,
            const user::auth::parseTokenReq *request,
            user::auth::parseTokenResp *response) override;
private:
    std::shared_ptr<UserServiceConfig> config;
};



#endif //AUTHSERVICEIMPL_H
