//
// Created by Fan on 25-4-28.
//

#ifndef USERSERVICECONFIG_H
#define USERSERVICECONFIG_H

#include "Config.h"
#include "Exception.h"

typedef struct {
    std::string secret;
    std::unordered_set<std::string> userID;
} ADMIN_CONFIG;

class UserServiceConfig : public Config {
public:
    explicit UserServiceConfig(const std::string &config) : Config(config), rpcConfig() {
        const std::shared_ptr<YAML::Node> configNode = getConfig();
        if ((*configNode)["rpc"]) {
            readRpcConfig((*configNode)["rpc"]);
        }
        if ((*configNode)["admin"]) {
            readAdminConfig((*configNode)["admin"]);
        }
    }

    RPC_CONFIG* getRPCConfig() {
        return &rpcConfig;
    }

    ADMIN_CONFIG* getAdminConfig() {
        return &adminConfig;
    }

private:
    void readRpcConfig(const YAML::Node& node) {
        rpcConfig.registerIP = node["registerIP"].as<std::string>();
        if (rpcConfig.registerIP.empty()) {
            rpcConfig.registerIP = "127.0.0.1";
        }
        rpcConfig.listenIP = node["listenIP"].as<std::string>();
        if (rpcConfig.listenIP.empty()) {
            rpcConfig.listenIP = "0.0.0.0";
        }
        rpcConfig.autoSetPorts = node["autoSetPorts"].as<bool>();
        if (rpcConfig.autoSetPorts) {
            rpcConfig.ports.push_back(RPC_DEFAULT_PORT_USER_SERVICE);
        }
        else if (node["ports"] && node["ports"].IsSequence()){
            for (auto port : node["ports"]) {
                rpcConfig.ports.push_back(port.as<int>());
            }
        }
        else {
            throw Exception("MsgGateWayConfig get rpc port error");
        }
    }

    void readAdminConfig(const YAML::Node& node) {
        adminConfig.secret = node["secret"].as<std::string>();
        if (adminConfig.secret.empty()) {
            adminConfig.secret = "admin";
        }
        adminConfig.userID.insert(node["userID"].as<std::string>());
        if (adminConfig.userID.empty()) {
            adminConfig.userID.insert("admin");
        }

    }

    RPC_CONFIG rpcConfig;
    ADMIN_CONFIG adminConfig;
};

#endif //USERSERVICECONFIG_H
