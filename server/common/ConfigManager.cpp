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

ConfigManager::ConfigManager(const CONFIG_TYPE configType) {
    initDefaultConfig(configType);
}

ConfigManager::ConfigManager(const std::vector<CONFIG_TYPE> &configTypes) {
    for (const auto configType : configTypes) {
        initDefaultConfig(configType);
    }
}

void ConfigManager::parseConfig(const CONFIG_TYPE configType,
                                const std::function<void(ConfigManager*, std::shared_ptr<YAML::Node> &)> &parse) {
    try {
        Config config(CONFIGTYPE_FILE_MAP[configType]);
        parse(this, config.getConfig());
    } catch (YAML::BadFile& e) { }
}

bool ConfigManager::hasConfig(const CONFIG_TYPE configType) const {
    bool result = false;
    switch (configType) {
        case CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY:
            result = (msgGatewayConfig != nullptr);
            break;
        case CONFIG_TYPE::CONFIG_TYPE_SERVICE_USER:
            result = (userServiceConfig != nullptr);
            break;
        case CONFIG_TYPE::CONFIG_TYPE_DB:
            result = (dbConfig != nullptr);
            break;
        default:
            break;
    }
    return result;
}

bool ConfigManager::hasConfigField(const std::shared_ptr<YAML::Node> &configNode, const std::string &fieldName) {
    return (configNode->IsDefined() && (*configNode)[fieldName].IsDefined() && !(*configNode)[fieldName].IsNull());
}

MSG_GATEWAY_CONFIG* ConfigManager::getMsgGatewayConfig() const {
    return msgGatewayConfig.get();
}

SERVICE_USER_CONFIG* ConfigManager::getUserServiceConfig() const {
    return userServiceConfig.get();
}

DB_CONFIG* ConfigManager::getDBConfig() const {
    return dbConfig.get();
}

void ConfigManager::initDefaultConfig(const CONFIG_TYPE configType) {
    switch (configType) {
        case CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY:
            initMsgGatewayConfig();
            initServiceUserConfig();
            break;
        case CONFIG_TYPE::CONFIG_TYPE_SERVICE_USER:
            initServiceUserConfig();
            break;
        case CONFIG_TYPE::CONFIG_TYPE_DB:
            initDBConfig();
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
    userServiceConfig = std::make_unique<SERVICE_USER_CONFIG>();
    userServiceConfig->adminUserID = "admin";
    userServiceConfig->adminSecret = "admin";
    userServiceConfig->serviceConfig.registerIP = "127.0.0.1";
    userServiceConfig->serviceConfig.listenIP = "0.0.0.0";
    userServiceConfig->serviceConfig.autoSetPorts = true;
    userServiceConfig->serviceConfig.ports.push_back(RPC_DEFAULT_PORT_USER_SERVICE);
}

void ConfigManager::initDBConfig() {
    dbConfig = std::make_unique<DB_CONFIG>();
    dbConfig->address = "";
    dbConfig->username = "";
    dbConfig->password = "";
    dbConfig->poolSize = 0;
    dbConfig->retryNum = 0;
}
