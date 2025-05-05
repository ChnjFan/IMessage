//
// Created by Fan on 25-5-5.
//

#ifndef AUTHCLIENT_H
#define AUTHCLIENT_H

#include <memory>

#include <grpcpp/grpcpp.h>
#include "auth/auth.grpc.pb.h"

class AuthClient {
public:
    explicit AuthClient(const std::shared_ptr<grpc::Channel> &channel);
    bool getAdminToken(std::string &userid, std::string &secret, std::string &token, int64_t &expireTime) const;

private:
    std::unique_ptr<user::auth::Auth::Stub> stub;
};



#endif //AUTHCLIENT_H
