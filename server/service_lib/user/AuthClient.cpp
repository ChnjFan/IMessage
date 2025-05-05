//
// Created by Fan on 25-5-5.
//

#include "AuthClient.h"

AuthClient::AuthClient(const std::shared_ptr<grpc::Channel> &channel)
        : stub(user::auth::Auth::NewStub(channel)) { }

bool AuthClient::getAdminToken(std::string &userid, std::string &secret, std::string &token, int64_t &expireTime) const {
    grpc::ClientContext context;
    user::auth::getAdminTokenReq request;
    request.set_userid(userid);
    request.set_secret(secret);

    user::auth::getAdminTokenResp response;
    bool result = false;
    bool done = false;
    std::mutex mu;
    std::condition_variable cv;
    stub->async()->getAdminToken(&context, &request, &response,
        [&result, &done, &mu, &cv, response, &token, &expireTime](grpc::Status status) {
            bool ret = false;
            if (!status.ok()) {
                ret = false;
            } else if (response.token().empty()) {
                ret = false;
            } else {
                token = response.token();
                expireTime = response.expiretimeseconds();
                ret = true;
            }
            std::lock_guard<std::mutex> lock(mu);
            result = ret;
            done = true;
            cv.notify_one();
        });
    std::unique_lock<std::mutex> lock(mu);
    cv.wait(lock, [&done] { return done; });
    return result;
}
