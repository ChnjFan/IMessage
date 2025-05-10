//
// Created by fan on 25-4-17.
//

#include "MsgGatewayServer.h"

MsgGatewayServer::MsgGatewayServer(MsgGateWay::MsgGateWayConfig &config, ConnServerPtr &server)
    : config(config)
    , longServer(server)
    , clientManager(nullptr) {
}

void MsgGatewayServer::init() {
    clientManager = std::make_shared<ClientManager>();
    clientManager->registerService(SERVICE_NAME::SERVICE_USER, config.getRpcConfig());

    // TODO:长链接用于获取用户、推送、认证和消息rpc服务链接

    // TODO:发布消息网管rpc服务
}

ConnServerPtr MsgGatewayServer::getConnServer() {
    return longServer;
}
