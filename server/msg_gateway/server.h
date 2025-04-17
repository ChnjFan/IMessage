//
// Created by fan on 25-4-13.
//

#ifndef IMSERVER_SERVER_H
#define IMSERVER_SERVER_H

#define MODULE_NAME "MSG_GATEWAY_SERVER"
#define LOGGER_CONFIG_NAME "imserver-msggateway.properties"
#define SERVER_CONFIG_NAME "msggateway.yaml"

extern std::shared_ptr<Logger> g_server_logger;

#define MSG_GATEWAY_SERVER_LOG_DEBUG(msg) \
do {\
    g_server_logger->debug(msg);\
} while(0)

#define MSG_GATEWAY_SERVER_LOG_INFO(msg) \
do {\
    g_server_logger->info(msg);\
} while(0)

#define MSG_GATEWAY_SERVER_LOG_ERROR(msg) \
do {\
    g_server_logger->error(msg);\
} while(0)

#endif //IMSERVER_SERVER_H
