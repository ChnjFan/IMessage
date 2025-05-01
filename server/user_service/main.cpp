//
// Created by Fan on 25-4-26.
//


#include "ServiceImpl.h"
#include "UserServiceUtil.h"
#include "UserServiceConfig.h"

std::shared_ptr<Logger> g_server_logger = nullptr;

int main() {
    try {
        g_server_logger = LoggerFactory::getLogger(LOGGER_CONFIG_NAME, MODULE_NAME);
        const auto config = std::make_shared<UserServiceConfig>(SERVER_CONFIG_NAME);

        const int rpcPort = config->getRPCConfig()->ports[0];
        USER_SERVICE_SERVER_LOG_INFO("Server is initializing. RPCPort: " + std::to_string(rpcPort));

        ServiceImpl service(config);
        service.run(config->getRPCConfig()->listenIP, rpcPort);
    }
    catch (Exception& e) {
        USER_SERVICE_SERVER_LOG_ERROR("Server error: " + std::string(e.what()));
    }
    return 0;
}
