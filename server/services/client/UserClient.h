//
// Created by Fan on 25-5-5.
//

#ifndef AUTHCLIENT_H
#define AUTHCLIENT_H

#include <memory>

#include <grpcpp/grpcpp.h>
#include "auth/auth.grpc.pb.h"
#include "model/UserInfo.h"

class UserClient {
public:
    explicit UserClient(const std::shared_ptr<grpc::Channel> &channel);

    bool getUserToken(const UserInfo *pUserInfo, std::string *token, int64_t *expireTime) const;

    static UserInfo* parseLoginRequest(const char *request);
    static UserInfo* parseRegisterRequest(const char *request);

private:
    std::unique_ptr<user::auth::Auth::Stub> stub;
};

#endif //AUTHCLIENT_H
