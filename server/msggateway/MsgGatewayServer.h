//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_MSGGATEWAYSERVER_H
#define IMSERVER_MSGGATEWAYSERVER_H

#include <memory>

#include "ConnServer.h"
#include "ClientManager.h"
#include "ConfigManager.h"

class MsgGatewayServer {
public:
    MsgGatewayServer(std::shared_ptr<ConfigManager>& config, ConnServerPtr& server);

    void init();
    ConnServerPtr getConnServer();

private:
    std::shared_ptr<ConfigManager> &config;
    ConnServerPtr longServer;
};


#endif //IMSERVER_MSGGATEWAYSERVER_H
