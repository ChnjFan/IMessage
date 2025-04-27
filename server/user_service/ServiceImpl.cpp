//
// Created by Fan on 25-4-27.
//

#include "ServiceImpl.h"
#include "CallData.h"

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

    completeQueue = builder.AddCompletionQueue();
    server = builder.BuildAndStart();

    handleUserService();
}

void ServiceImpl::handleUserService() {
    // 为每个服务创建实例
    new CallData<user::auth::Auth::AsyncService, user::auth::getAdminTokenReq, user::auth::getAdminTokenResp>(
        &authService, completeQueue.get());
    new CallData<user::auth::Auth::AsyncService, user::auth::parseTokenReq, user::auth::parseTokenResp>(
        &authService, completeQueue.get());

    void* tag = nullptr;
    bool ok = false;
    while (completeQueue->Next(&tag, &ok)) {
        if (!ok) {
            // TODO:错误处理
            continue;
        }
        static_cast<BaseCallData*>(tag)->Proceed();
    }
}
