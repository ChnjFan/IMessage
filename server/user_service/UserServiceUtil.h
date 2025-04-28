//
// Created by Fan on 25-4-28.
//

#ifndef USERSERVICEUTIL_H
#define USERSERVICEUTIL_H

#include "Logger.h"

#define MODULE_NAME "USER_SERVICE_SERVER"
#define LOGGER_CONFIG_NAME "imserver-userservice.properties"
#define SERVER_CONFIG_NAME "userservice.yaml"

extern std::shared_ptr<Logger> g_server_logger;

#define USER_SERVICE_SERVER_LOG_DEBUG(msg) \
do {\
    g_server_logger->debug(msg);\
} while(0)

#define USER_SERVICE_SERVER_LOG_INFO(msg) \
do {\
    g_server_logger->info(msg);\
} while(0)

#define USER_SERVICE_SERVER_LOG_WARN(msg) \
do {\
    g_server_logger->warn(msg);\
} while(0)

#define USER_SERVICE_SERVER_LOG_ERROR(msg) \
do {\
    g_server_logger->error(msg);\
} while(0)

#endif //USERSERVICEUTIL_H
