//
// Created by Fan on 25-4-27.
//

#ifndef SERVICEIMPL_H
#define SERVICEIMPL_H

#include <grpcpp/grpcpp.h>

#include "AuthServiceImpl.h"

class ServiceImpl final {
public:
    ~ServiceImpl();

    void run(std::string& ip, int port);

private:
    // rpc 服务
    AuthServiceImpl authService;

    std::unique_ptr<grpc::ServerCompletionQueue> completeQueue;
    std::unique_ptr<grpc::Server> server;
};



#endif //SERVICEIMPL_H
