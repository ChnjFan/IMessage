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
        pConfig->ports.push_back(RPC_DEFAULT_PORT_MSG_GATEWAY);
    }
    else {
        pConfig->ports.push_back(node["port"].as<int>());
    }
}

void loadLongConnConfig(MSG_GATEWAY_CONFIG *pConfig, YAML::Node &node) {
    if (nullptr == pConfig) {
        return;
    }
    if (node["ports"] && node["ports"].IsSequence()){
        for (auto port : node["ports"]) {
            pConfig->ports.push_back(port.as<int>());
        }
    }
    pConfig->socketMaxConnNum = node["socketMaxConnNum"].as<int>();
    pConfig->socketMaxMsgLen = node["socketMaxMsgLen"].as<int>();
    pConfig->socketTimeout = node["socketTimeout"].as<int>();
}

std::shared_ptr<ConfigManager> loadConfig() {
    std::vector configTypes = {
        CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY,
        CONFIG_TYPE::CONFIG_TYPE_DB,
    };
    auto configMgr = std::make_shared<ConfigManager>(configTypes);
    configMgr->parseConfig(CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY,
        [](const ConfigManager *mgr, const std::shared_ptr<YAML::Node> &configNode) {
            if (ConfigManager::hasConfigField(configNode, "rpc")) {
                YAML::Node rpcNode = (*configNode)["rpc"];
                loadRpcServiceConfig(&mgr->getMsgGatewayConfig()->serviceConfig, rpcNode);
            }

            if (ConfigManager::hasConfigField(configNode, "longConnSvr")) {
                YAML::Node longConnNode = (*configNode)["longConnSvr"];
                loadLongConnConfig(mgr->getMsgGatewayConfig(), longConnNode);
            }

            if (ConfigManager::hasConfigField(configNode, "listenIP")) {
                mgr->getMsgGatewayConfig()->listenIP = (*configNode)["listenIP"].as<std::string>();
            }
            else {
                mgr->getMsgGatewayConfig()->listenIP = "0.0.0.0";
            }
        });
    configMgr->parseConfig(CONFIG_TYPE::CONFIG_TYPE_DB,
        [](const ConfigManager *mgr, const std::shared_ptr<YAML::Node> &configNode) {
            DB_CONFIG *dbConfig = mgr->getDBConfig();
            if (nullptr == dbConfig) {
                return;
            }
            if (ConfigManager::hasConfigField(configNode, "username")) {
                dbConfig->address = (*configNode)["address"].as<std::string>();
            }
            if (ConfigManager::hasConfigField(configNode, "password")) {
                dbConfig->password = (*configNode)["password"].as<std::string>();
            }
            if (ConfigManager::hasConfigField(configNode, "maxPoolSize")) {
                dbConfig->poolSize = (*configNode)["maxPoolSize"].as<int>();
            }
            if (ConfigManager::hasConfigField(configNode, "maxRetry")) {
                dbConfig->retryNum = (*configNode)["maxRetry"].as<int>();
            }
        });
    return configMgr;
}

int main() {
    try {
        g_server_logger = LoggerFactory::getLogger(LOGGER_CONFIG_NAME, MODULE_NAME);

        auto configMgr = loadConfig();

        const int longPort = configMgr->getMsgGatewayConfig()->ports[0]; //多个实例时需要计算当前端口
        MSG_GATEWAY_SERVER_LOG_INFO("Server is initializing. longConnSvrPort: "
                                    + std::to_string(longPort));
        //TODO:连接数据库

        boost::asio::io_context io_context;
        ConnServerPtr longServer = ConnServerFactory::getConnServer(io_context,
                configMgr,
                longPort,
                configMgr->getMsgGatewayConfig()->socketMaxConnNum,
                configMgr->getMsgGatewayConfig()->socketMaxMsgLen,
                configMgr->getMsgGatewayConfig()->socketTimeout);

        MsgGatewayServer server(configMgr, longServer);
        server.init();

        server.getConnServer()->run();
        io_context.run();
    }
    catch (Exception& e) {
        MSG_GATEWAY_SERVER_LOG_ERROR("Server error: " + std::string(e.what()));
    }

    return 0;
}