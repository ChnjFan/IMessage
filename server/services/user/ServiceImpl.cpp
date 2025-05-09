//
// Created by Fan on 25-4-27.
//

#include "ServiceImpl.h"
#include "UserServiceUtil.h"

ServiceImpl::ServiceImpl(const std::shared_ptr<UserServiceConfig> &config) : authService(config) {
}

ServiceImpl::~ServiceImpl() {
    server->Shutdown();
    completeQueue->Shutdown();
}

void ServiceImpl::run(std::string& ip, int port) {
    const std::string serverAddress = ip + ":" + std::to_string(port);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    // 注册 rpc 服务
    builder.RegisterService(&authService);

    const std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    USER_SERVICE_SERVER_LOG_INFO("Server listening on " + serverAddress);
    server->Wait();
}
