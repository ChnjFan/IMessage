//
// Created by Fan on 25-5-5.
//

#ifndef AUTHCLIENT_H
#define AUTHCLIENT_H

#include <memory>

#include <grpcpp/grpcpp.h>
#include "auth/auth.grpc.pb.h"

typedef struct {
    std::string userID;
    std::string secret;
    std::string token;
    std::string nickName;
    std::string faceURL;
    int64_t expireTime;
} USER_SERVICE_INFO;

#define USER_CLIENT_ADMIN_ID "imAdmin"

class UserClient {
public:
    explicit UserClient(const std::shared_ptr<grpc::Channel> &channel);

    bool getAdminToken(const std::string &userid, const std::string &secret, std::string *token, int64_t *expireTime);

    static USER_SERVICE_INFO* parseLoginRequest(const char *request);
    static USER_SERVICE_INFO* parseRegisterRequest(const char *request);

    static bool isAdminID(const std::string &userID);

private:
    std::unique_ptr<user::auth::Auth::Stub> stub;
};

#endif //AUTHCLIENT_H
