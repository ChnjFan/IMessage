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

class Config {
public:
    explicit Config(const std::string &file);

    std::shared_ptr<YAML::Node> getConfig();

private:
    std::shared_ptr<YAML::Node> config;
};

#endif //IMSERVER_CONFIG_H
