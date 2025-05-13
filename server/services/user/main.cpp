//
// Created by Fan on 25-4-26.
//


#include "ConfigManager.h"
#include "Exception.h"
#include "ServiceImpl.h"
#include "UserServiceUtil.h"

std::shared_ptr<Logger> g_server_logger = nullptr;

void loadRpcServiceConfig(RPC_SERVICE_CONFIG *pConfig, YAML::Node &node) {
    if (nullptr == pConfig) {
        return;
    }
    pConfig->registerIP = node["registerIP"].as<std::string>();
    if (pConfig->registerIP.empty()) {
        pConfig->registerIP = "127.0.0.1";
    }
    pConfig->autoSetPorts = node["autoSetPorts"].as<bool>();
    if (pConfig->autoSetPorts) {
        pConfig->ports.push_back(RPC_DEFAULT_PORT_USER_SERVICE);
    }
    else {
        pConfig->ports.push_back(node["port"].as<int>());
    }
}

std::shared_ptr<ConfigManager> loadConfig() {
    auto config = std::make_shared<ConfigManager>(CONFIG_TYPE::CONFIG_TYPE_SERVICE_USER);
    config->parseConfig(CONFIG_TYPE::CONFIG_TYPE_SERVICE_USER,
        [](const ConfigManager *mgr, const std::shared_ptr<YAML::Node> &configNode) {
            if (ConfigManager::hasConfigField(configNode, "rpc")) {
                YAML::Node rpcNode = (*configNode)["rpc"];
                loadRpcServiceConfig(&mgr->getUserServiceConfig()->serviceConfig, rpcNode);
            }
        });
    return config;
}

int main() {
    try {
        g_server_logger = LoggerFactory::getLogger(LOGGER_CONFIG_NAME, MODULE_NAME);
        const auto config = loadConfig();

        const auto pUserConfig = config->getUserServiceConfig();
        if (nullptr == pUserConfig) {
            return EXIT_FAILURE;
        }
        const int rpcPort = pUserConfig->serviceConfig.ports[0];
        USER_SERVICE_SERVER_LOG_INFO("Server is initializing. RPCPort: " + std::to_string(rpcPort));

        ServiceImpl service(config);
        service.run(pUserConfig->serviceConfig.listenIP, rpcPort);
    }
    catch (Exception& e) {
        USER_SERVICE_SERVER_LOG_ERROR("Server error: " + std::string(e.what()));
    }
    return 0;
}
