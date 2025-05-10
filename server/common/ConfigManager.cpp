//
// Created by Fan on 25-5-10.
//

#include "ConfigManager.h"
#include "util.h"

class Config {
public:
    explicit Config(const std::string &file) {
        const std::string fullConfigFile = getProjectPath() + SERVER_CONFIG_PATH + file;
        config = std::make_shared<YAML::Node>(YAML::LoadFile(fullConfigFile));
    }

    std::shared_ptr<YAML::Node> &getConfig() {
        return config;
    }

private:
    std::shared_ptr<YAML::Node> config;
};

ConfigManager::ConfigManager(CONFIG_TYPE configType) {
    initDefaultConfig(configType);
}

void ConfigManager::parseConfig(CONFIG_TYPE configType,
    const std::function<void(ConfigManager*, std::shared_ptr<YAML::Node> &)> &parse) {
    try {
        Config config(CONFIGTYPE_FILE_MAP[configType]);
        parse(this, config.getConfig());
    } catch (YAML::BadFile& e) { }
}

MSG_GATEWAY_CONFIG* ConfigManager::getMsgGatewayConfig() const {
    return msgGatewayConfig.get();
}

RPC_SERVICE_CONFIG* ConfigManager::getUserServiceConfig() const {
    return userServiceConfig.get();
}

void ConfigManager::initDefaultConfig(CONFIG_TYPE configType) {
    switch (configType) {
        case CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY:
            initMsgGatewayConfig();
            initServiceUserConfig();
            break;
        case CONFIG_TYPE::CONFIG_TYPE_SERVICE_USER:
            initServiceUserConfig();
            break;
        default:
            break;
    }
}

void ConfigManager::initMsgGatewayConfig() {
    msgGatewayConfig = std::make_unique<MSG_GATEWAY_CONFIG>();
    msgGatewayConfig->listenIP = "0.0.0.0";
    // 网关服务配置
    msgGatewayConfig->serviceConfig.registerIP = "127.0.0.1";
    msgGatewayConfig->serviceConfig.listenIP = "0.0.0.0";
    msgGatewayConfig->serviceConfig.autoSetPorts = true;
    msgGatewayConfig->serviceConfig.ports.push_back(RPC_DEFAULT_PORT_MSG_GATEWAY);
    // 长链接服务配置
    msgGatewayConfig->ports.push_back(SERVER_DEFAULT_PORT);
    msgGatewayConfig->socketMaxConnNum = SERVER_MAX_CONNECTIONS;
    msgGatewayConfig->socketMaxMsgLen = SERVER_MAX_MSG_LEN;
    msgGatewayConfig->socketTimeout = SERVER_MAX_SOCKET_TIMEOUT;
}

void ConfigManager::initServiceUserConfig() {
    userServiceConfig = std::make_unique<RPC_SERVICE_CONFIG>();
    userServiceConfig->registerIP = "127.0.0.1";
    userServiceConfig->listenIP = "0.0.0.0";
    userServiceConfig->autoSetPorts = true;
    userServiceConfig->ports.push_back(RPC_DEFAULT_PORT_USER_SERVICE);
}
