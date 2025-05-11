//
// Created by Fan on 25-5-10.
//

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#define SERVER_CONFIG_PATH "config/"            // 配置目录

//IMServer 默认监听端口
#define SERVER_DEFAULT_PORT             10000
#define SERVER_MAX_CONNECTIONS          100000  // 服务器实例默认最大 10k 连接
#define SERVER_MAX_MSG_LEN              4096    // 服务器实例默认最大消息长度
#define SERVER_MAX_SOCKET_TIMEOUT       10      // 服务器实例默认最大超时时间（s）

// 服务默认端口
#define RPC_DEFAULT_PORT_MSG_GATEWAY    10001   // 消息网关默认监听端口
#define RPC_DEFAULT_PORT_USER_SERVICE   11001   // 用户服务默认监听端口

enum class CONFIG_TYPE {
    CONFIG_TYPE_NONE,
    CONFIG_TYPE_MSG_GATEWAY,
    CONFIG_TYPE_SERVICE_USER,
};

inline std::unordered_map<CONFIG_TYPE, std::string> CONFIGTYPE_FILE_MAP = {
    {CONFIG_TYPE::CONFIG_TYPE_NONE, ""},
    {CONFIG_TYPE::CONFIG_TYPE_MSG_GATEWAY, "msggateway.yaml"},
    {CONFIG_TYPE::CONFIG_TYPE_SERVICE_USER, "userservice.yaml"},
};

// rpc 服务配置
typedef struct {
    std::string registerIP;     // 默认为 127.0.0.1
    std::string listenIP;
    bool autoSetPorts;
    std::vector<int> ports;
} RPC_SERVICE_CONFIG;

// 消息网管配置
typedef struct {
    std::string listenIP;
    RPC_SERVICE_CONFIG serviceConfig;
    // 连接服务配置
    std::vector<int> ports;
    int socketMaxConnNum;
    int socketMaxMsgLen;
    int socketTimeout;
} MSG_GATEWAY_CONFIG;

class ConfigManager {
public:
    explicit ConfigManager(CONFIG_TYPE configType);

    void parseConfig(CONFIG_TYPE configType, const std::function<void(ConfigManager*, std::shared_ptr<YAML::Node>&)> &parse);

    [[nodiscard]] bool hasConfig(CONFIG_TYPE configType) const;
    static bool hasConfigField(const std::shared_ptr<YAML::Node>& configNode, const std::string& fieldName);

    [[nodiscard]] MSG_GATEWAY_CONFIG* getMsgGatewayConfig() const;
    [[nodiscard]] RPC_SERVICE_CONFIG* getUserServiceConfig() const;

private:
    void initDefaultConfig(CONFIG_TYPE configType);
    void initMsgGatewayConfig();
    void initServiceUserConfig();

    std::unique_ptr<MSG_GATEWAY_CONFIG> msgGatewayConfig;
    std::unique_ptr<RPC_SERVICE_CONFIG> userServiceConfig;
};


#endif //CONFIGMANAGER_H
