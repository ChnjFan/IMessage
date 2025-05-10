//
// Created by Fan on 25-5-10.
//

#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <grpcpp/grpcpp.h>

/* 服务定义 */
enum class SERVICE_NAME {
    SERVICE_USER = 1,
};

class ClientInfo;

class ClientManager {
public:
    // ClientManager(/*config*/);
    void registerService(SERVICE_NAME serviceName, const std::string& target);

    std::shared_ptr<grpc::Channel>& getChannel(SERVICE_NAME serviceName);

    std::string getServiceName(SERVICE_NAME serviceName);

private:
    std::unordered_map<SERVICE_NAME, std::string> serviceNameMap;
    std::unordered_map<SERVICE_NAME, std::shared_ptr<grpc::Channel>> channels;   // 管理 grpc 连接
};



#endif //CLIENTMANAGER_H
