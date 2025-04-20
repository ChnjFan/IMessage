//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_MSGGATEWAYSERVER_H
#define IMSERVER_MSGGATEWAYSERVER_H

#include <memory>

#include "MsgGateWayConfig.h"
#include "ConnServer.h"

class MsgGatewayServer {
public:
    MsgGatewayServer(MsgGateWay::MsgGateWayConfig& config, ConnServerPtr& server);

    void init();
    ConnServerPtr getConnServer();

private:
    MsgGateWay::MsgGateWayConfig &config;
    ConnServerPtr longServer;

    /* TODO:grpc链接 */

};


#endif //IMSERVER_MSGGATEWAYSERVER_H
