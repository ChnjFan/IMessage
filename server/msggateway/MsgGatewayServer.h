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
    MsgGatewayServer(std::shared_ptr<ConfigManager>& config, const ConnServerPtr& server);

    void init();
    void run(io_context & io_context);

private:
    std::shared_ptr<ConfigManager> &config;
    ConnServerPtr connServer;
};


#endif //IMSERVER_MSGGATEWAYSERVER_H
