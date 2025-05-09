//
// Created by fan on 25-4-13.
//

#ifndef IMSERVER_MSGGATEWAYCONFIG_H
#define IMSERVER_MSGGATEWAYCONFIG_H

#include "Config.h"
#include "Exception.h"

namespace MsgGateWay {

typedef struct {
    std::vector<int> ports;
    int socketMaxConnNum;
    int socketMaxMsgLen;
    int socketTimeout;
} LONG_SERVER_CONFIG;

typedef struct {
    std::string listenIP;
    RPC_CONFIG rpcConfig;
    LONG_SERVER_CONFIG longServerConfig;
} MSG_GATEWAY_CONFIG;

class MsgGateWayConfig : public Config {
public:
    explicit MsgGateWayConfig(const std::string& file) : Config(file), config() {
        const std::shared_ptr<YAML::Node> configNode = getConfig();
        if ((*configNode)["rpc"]) {
            readRpcConfig((*configNode)["rpc"]);
        }
        if ((*configNode)["longConnSvr"]) {
            readLongServerConfig((*configNode)["longConnSvr"]);
        }
        if ((*configNode)["listenIP"]) {
            config.listenIP = (*configNode)["listenIP"].as<std::string>();
        }
    }

    void readRpcConfig(const YAML::Node& node) {
        config.rpcConfig.registerIP = node["registerIP"].as<std::string>();
        config.rpcConfig.autoSetPorts = node["autoSetPorts"].as<bool>();
        if (config.rpcConfig.autoSetPorts) {
            config.rpcConfig.ports.push_back(RPC_DEFAULT_PORT_MSG_GATEWAY);
        }
        else if (node["ports"] && node["ports"].IsSequence()){
            for (auto port : node["ports"]) {
                config.rpcConfig.ports.push_back(port.as<int>());
            }
        }
        else {
            throw Exception("MsgGateWayConfig get rpc port error");
        }
    }

    RPC_CONFIG* getRpcConfig() {
        return &config.rpcConfig;
    }

    void readLongServerConfig(const YAML::Node& node) {
        if (node["ports"] && node["ports"].IsSequence()){
            for (auto port : node["ports"]) {
                config.longServerConfig.ports.push_back(port.as<int>());
            }
        }
        else {
            throw Exception("MsgGateWayConfig get rpc port error");
        }
        config.longServerConfig.socketMaxConnNum = node["socketMaxConnNum"].as<int>();
        config.longServerConfig.socketMaxMsgLen = node["socketMaxMsgLen"].as<int>();
        config.longServerConfig.socketTimeout = node["socketTimeout"].as<int>();
    }

    LONG_SERVER_CONFIG* getLongServerConfig() {
        return &config.longServerConfig;
    }

    [[nodiscard]] std::string getListenIP() const {
        return config.listenIP;
    }
private:
    MSG_GATEWAY_CONFIG config;
};

}

#endif //IMSERVER_MSGGATEWAYCONFIG_H
