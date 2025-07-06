//
// Created by fan on 25-4-17.
//

#include "MsgGatewayServer.h"

MsgGatewayServer::MsgGatewayServer(std::shared_ptr<ConfigManager> &config, const ConnServerPtr &server)
    : config(config)
    , connServer(server) {
}

void MsgGatewayServer::init() {
    // 连接 DB 服务器
    // TODO:长链接用于获取用户、推送、认证和消息rpc服务链接

    // TODO:发布消息网管rpc服务
}

void MsgGatewayServer::run(io_context &io_context) {
    connServer->run(io_context);
    io_context.run();
}

