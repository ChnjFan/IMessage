//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_MSGGATEWAYSERVER_H
#define IMSERVER_MSGGATEWAYSERVER_H

#include <memory>

#include "ConnServer.h"
#include "ClientManager.h"

class MsgGatewayServer {
public:
    MsgGatewayServer(MsgGateWay::MsgGateWayConfig& config, ConnServerPtr& server);

    void init();
    ConnServerPtr getConnServer();

private:
    MsgGateWay::MsgGateWayConfig &config;
    ConnServerPtr longServer;

    /* TODO:grpc链接 */
    std::shared_ptr<ClientManager> clientManager;  // 用户 rpc 服务

};


#endif //IMSERVER_MSGGATEWAYSERVER_H
