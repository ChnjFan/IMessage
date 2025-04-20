//
// Created by fan on 25-4-13.
//

#include "Logger.h"
#include "server.h"
#include "Exception.h"
#include "ConnServer.h"
#include "MsgGatewayServer.h"
#include "MsgGateWayConfig.h"

std::shared_ptr<Logger> g_server_logger = nullptr;

int main() {
    try {
        g_server_logger = LoggerFactory::getLogger(LOGGER_CONFIG_NAME, MODULE_NAME);
        MsgGateWay::MsgGateWayConfig config(SERVER_CONFIG_NAME);
        int longPort = config.getLongServerConfig()->ports[0]; //多个实例时需要计算当前端口
        MSG_GATEWAY_SERVER_LOG_INFO("Server is initializing. longConnSvrPort: "
                                    + std::to_string(longPort));
        //TODO:连接数据库

        ConnServerPtr longServer = ConnServerFactory::getConnServer(longPort,
                                                                    config.getLongServerConfig()->socketMaxConnNum,
                                                                    config.getLongServerConfig()->socketMaxMsgLen,
                                                                    config.getLongServerConfig()->socketTimeout);

        MsgGatewayServer server(config, longServer);
        server.init();
        server.getConnServer()->run();
    }
    catch (Exception& e) {
        MSG_GATEWAY_SERVER_LOG_ERROR("Server error: " + std::string(e.what()));
    }

    return 0;
}