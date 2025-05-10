//
// Created by fan on 25-4-13.
//

#include <boost/asio.hpp>

#include "Logger.h"
#include "Exception.h"
#include "ConfigManager.h"
#include "ConnServer.h"
#include "MsgGatewayUtil.h"
#include "MsgGatewayServer.h"

std::shared_ptr<Logger> g_server_logger = nullptr;

int main() {
    try {
        g_server_logger = LoggerFactory::getLogger(LOGGER_CONFIG_NAME, MODULE_NAME);
        ConfigManager config(CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY, [](std::shared_ptr<YAML::Node>& config) {

        });
        const int longPort = config.getMsgGatewayConfig()->ports[0]; //多个实例时需要计算当前端口
        MSG_GATEWAY_SERVER_LOG_INFO("Server is initializing. longConnSvrPort: "
                                    + std::to_string(longPort));
        //TODO:连接数据库

        boost::asio::io_context io_context;
        ConnServerPtr longServer = ConnServerFactory::getConnServer(io_context,
                longPort,
                config.getLongServerConfig()->socketMaxConnNum,
                config.getLongServerConfig()->socketMaxMsgLen,
                config.getLongServerConfig()->socketTimeout);

        MsgGatewayServer server(config, longServer);
        server.init();

        server.getConnServer()->run();
        io_context.run();
    }
    catch (Exception& e) {
        MSG_GATEWAY_SERVER_LOG_ERROR("Server error: " + std::string(e.what()));
    }

    return 0;
}