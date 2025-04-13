//
// Created by fan on 25-4-13.
//

#ifndef IMESSAGE_LOGGER_H
#define IMESSAGE_LOGGER_H

#include <string>
#include <memory>

#define LOG_CONFIG_PATH "config/log/"

class LoggerImpl;

class Logger {
public:
    Logger(const std::string& config, const std::string& module);
    ~Logger();

    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

private:
    LoggerImpl *para;
};

class LoggerFactory {
public:
    static std::shared_ptr<Logger> getLogger(const std::string& config, const std::string& module);
};


#endif //IMESSAGE_LOGGER_H
