//
// Created by fan on 25-4-17.
//

#include "MsgGatewayServer.h"

MsgGatewayServer::MsgGatewayServer(MsgGateWay::MsgGateWayConfig &config, ConnServerPtr &server)
    : config(config)
    , longServer(server){

}

void MsgGatewayServer::init() {

}

void MsgGatewayServer::run() {
    longServer->run();
}
