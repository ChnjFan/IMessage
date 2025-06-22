//
// Created by Fan on 25-4-28.
//

#include <jwt-cpp/jwt.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "AuthServiceImpl.h"
#include "UserServiceUtil.h"
#include "boost/json/impl/kind.ipp"

AuthServiceImpl::AuthServiceImpl(const std::shared_ptr<ConfigManager> &config)
    : config(config)
    , userDB(config->getDBConfig()){
}

UserDatabase& AuthServiceImpl::getUserDatabase() {
    return userDB;
}

grpc::ServerUnaryReactor * AuthServiceImpl::getUserToken(grpc::CallbackServerContext *context,
                                                         const user::auth::getUserTokenReq *request,
                                                         user::auth::getUserTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(AuthServiceImpl& service,
                const user::auth::getUserTokenReq *request,
                user::auth::getUserTokenResp *response,
                const std::shared_ptr<ConfigManager>& config) {
            const UserInfo* pUserInfo = service.getUserDatabase().find(request->userid());
            if (nullptr == pUserInfo) {
                USER_SERVICE_SERVER_LOG_INFO("Login error: Not find user " + request->userid());
                Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, "Login error: Not find user"));
                return;
            }

            if (pUserInfo->getSecret() != request->secret()) {
                USER_SERVICE_SERVER_LOG_INFO("Login error: Wrong secret");
                Finish(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Login error: Wrong secret"));
                return;
            }

            std::string token = generateToken(std::to_string(request->userid()),
                config->getUserServiceConfig()->tokenSecret);
            response->set_token(token);
            response->set_expiretimeseconds(USER_TOKEN_EXPIRE_TIME);

            Finish(grpc::Status::OK);
        }

        static std::string generateToken(const std::string& userID, const std::string& secret) {
            boost::uuids::random_generator gen;
            auto token = jwt::create()
                .set_issuer("IMServer")
                .set_subject(userID)
                .set_audience("IMClient")
                .set_issued_at(std::chrono::system_clock::now())
                .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
                .set_not_before(std::chrono::system_clock::now())
                .set_id(boost::uuids::to_string(gen()))
                .sign(jwt::algorithm::hs256(secret));
            return token;
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
    return new Reactor(*this, request, response, config);
}

grpc::ServerUnaryReactor * AuthServiceImpl::parseToken(grpc::CallbackServerContext *context,
                                                       const user::auth::parseTokenReq *request, user::auth::parseTokenResp *response) {
    class Reactor : public grpc::ServerUnaryReactor {
    public:
        Reactor(const user::auth::parseTokenReq *request, user::auth::parseTokenResp *response,
                const std::shared_ptr<ConfigManager>& config) {
            try {
                auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256(config->getUserServiceConfig()->tokenSecret))
                    .with_issuer("IMServer")
                    .expires_at_leeway(30);
                const auto decoded_token = jwt::decode(request->token());
                const int32_t userID = std::stoi(decoded_token.get_subject());
                response->set_userid(userID);
            } catch (const std::exception &e) {
                USER_SERVICE_SERVER_LOG_WARN(e.what());
                Finish(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid token"));
            }
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
    return new Reactor(request, response, config);
}
