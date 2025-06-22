//
// Created by Fan on 25-5-5.
//

#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include "UserClient.h"

UserClient::UserClient(const std::shared_ptr<grpc::Channel> &channel)
        : stub(user::auth::Auth::NewStub(channel)) { }

bool UserClient::getUserToken(const UserInfo *pUserInfo, std::string *token, int64_t *expireTime) const {
    grpc::ClientContext context;
    user::auth::getUserTokenReq request;
    request.set_userid(userid);
    request.set_secret(secret);

    user::auth::getUserTokenResp response;
    bool result = false;
    bool done = false;
    std::mutex mu;
    std::condition_variable cv;
    stub->async()->getUserToken(&context, &request, &response,
        [&result, &done, &mu, &cv, &response, token, expireTime](const grpc::Status &status) {
            bool ret = false;
            if (!status.ok()) ret = false;
            else if (response.token().empty()) ret = false;
            else {
                *token = response.token();
                *expireTime = response.expiretimeseconds();
                ret = true;
            }
            std::lock_guard lock(mu);
            result = ret;
            done = true;
            cv.notify_one();
        });
    std::unique_lock lock(mu);
    cv.wait(lock, [&done] { return done; });
    return result;
}

UserInfo* UserClient::parseLoginRequest(const char *request) {
    boost::json::value json = boost::json::parse(request);
    const int32_t userid = static_cast<int32_t>(json.at("userid").as_int64());
    auto *response = new UserInfo(userid);
    std::string secret;
    secret = json.at("userID").as_string();
    response->setSecret(secret);
    return response;
}

UserInfo * UserClient::parseRegisterRequest(const char *request) {
    boost::json::value json = boost::json::parse(request);
    const int32_t userid = static_cast<int32_t>(json.at("userid").as_int64());
    auto *response = new UserInfo(userid);
    std::string value;
    value = json.at("nickName").as_string();
    response->setNickName(value);
    value = json.at("faceURL").as_string();
    response->setFaceURL(value);
    return response;
}
