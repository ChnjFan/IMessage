//
// Created by fan on 25-4-13.
//

#include "Config.h"
#include "util.h"

Config::Config(const std::string &file) {
    std::string fullConfigFile = getProjectPath() + SERVER_CONFIG_PATH + file;
    config = std::make_shared<YAML::Node>(YAML::LoadFile(fullConfigFile));
}

std::shared_ptr<YAML::Node> Config::getConfig() {
    return config;
}
