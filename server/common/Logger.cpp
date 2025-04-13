//
// Created by fan on 25-4-13.
//

#include "Logger.h"
#include <utility>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/propertyconfigurator.h>

#include "util.h"
#include "Exception.h"

class LoggerImpl {
public:
    explicit LoggerImpl(log4cxx::LoggerPtr logger) : logger_(std::move(logger)) {}

    log4cxx::LoggerPtr getLogger() {
        return logger_;
    }
private:
    log4cxx::LoggerPtr logger_;
};

Logger::Logger(const std::string& config, const std::string& module) : para(nullptr) {
    try {
        log4cxx::PropertyConfigurator::configure(config);
        para = new LoggerImpl(log4cxx::Logger::getLogger(module));
    } catch (log4cxx::helpers::Exception& e) {
        throw Exception("log4cxx init error: " + std::string(e.what()));
    }
}

Logger::~Logger() {
    delete para;
}

void Logger::debug(const std::string &message) {
    LOG4CXX_DEBUG(para->getLogger(), message);
}

void Logger::info(const std::string &message) {
    LOG4CXX_INFO(para->getLogger(), message);
}

void Logger::warn(const std::string &message) {
    LOG4CXX_WARN(para->getLogger(), message);
}

void Logger::error(const std::string &message) {
    LOG4CXX_ERROR(para->getLogger(), message);
}

std::shared_ptr<Logger> LoggerFactory::getLogger(const std::string &config, const std::string &module) {
    std::string configFile = getProjectPath() + LOG_CONFIG_PATH + config;
    return std::make_shared<Logger>(configFile, module);
}
