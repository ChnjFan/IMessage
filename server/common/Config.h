//
// Created by fan on 25-4-13.
//

#ifndef IMSERVER_CONFIG_H
#define IMSERVER_CONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>

#define SERVER_CONFIG_PATH "config/"

#define RPC_DEFAULT_PORT_MSG_GATEWAY    10000
#define RPC_DEFAULT_PORT_USER_SERVICE   11000

typedef struct {
    std::string registerIP;     // 默认为 127.0.0.1
    std::string listenIP;
    bool autoSetPorts;
    std::vector<int> ports;
} RPC_CONFIG;

class Config {
public:
    explicit Config(const std::string &file);

    std::shared_ptr<YAML::Node> getConfig();

private:
    std::shared_ptr<YAML::Node> config;
};

#endif //IMSERVER_CONFIG_H
