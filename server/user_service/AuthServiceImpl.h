//
// Created by Fan on 25-4-28.
//

#ifndef AUTHSERVICEIMPL_H
#define AUTHSERVICEIMPL_H

#include <grpcpp/grpcpp.h>

#include "auth/auth.grpc.pb.h"

class AuthServiceImpl final : public user::auth::Auth::CallbackService {
public:
    AuthServiceImpl();

    grpc::ServerUnaryReactor* getAdminToken(grpc::CallbackServerContext *context,
            const user::auth::getAdminTokenReq *request,
            user::auth::getAdminTokenResp *response) override;

    grpc::ServerUnaryReactor* parseToken(grpc::CallbackServerContext *context,
            const user::auth::parseTokenReq *request,
            user::auth::parseTokenResp *response) override;
};



#endif //AUTHSERVICEIMPL_H
