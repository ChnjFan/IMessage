//
// Created by Fan on 25-5-10.
//

#include "ClientManager.h"
#include "Exception.h"

void ClientManager::registerService(SERVICE_NAME serviceName, const std::string &target) {
    auto channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    channels.insert({serviceName, channel});
}

std::shared_ptr<grpc::Channel>& ClientManager::getChannel(SERVICE_NAME serviceName) {
    const auto it = channels.find(serviceName);
    if (it == channels.end()) {
        throw Exception("No such service '" + getServiceName(serviceName));
    }
    return it->second;
}

std::string ClientManager::getServiceName(SERVICE_NAME serviceName) {
    return serviceNameMap.at(serviceName);
}
