//
// Created by fan on 25-4-13.
//

#include <vector>
#include "Logger.h"
#include "MsgGateWayConfig.h"
#include "server.h"
#include "LongServer.h"

std::shared_ptr<Logger> g_server_logger = nullptr;

int main() {
    g_server_logger = LoggerFactory::getLogger(LOGGER_CONFIG_NAME, MODULE_NAME);
    MsgGateWay::MsgGateWayConfig config(SERVER_CONFIG_NAME);
    int longPort = config.getLongServerConfig()->ports[0]; //多个实例时需要计算当前端口
    MSG_GATEWAY_SERVER_LOG_INFO("[MSG_GATEWAY] Server is initializing. longConnSvrPort: "
                                + std::to_string(longPort));
    //TODO:连接数据库

    std::shared_ptr<MsgGateWay::LongServer> longServer = MsgGateWay::LongServerFactory::getLongServer(longPort,
        config.getLongServerConfig()->socketMaxConnNum,
        config.getLongServerConfig()->socketMaxMsgLen,
        config.getLongServerConfig()->socketTimeout);
    return 0;
}