//
// Created by fan on 25-4-13.
//

#ifndef IMSERVER_CONNSERVER_H
#define IMSERVER_CONNSERVER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <boost/pool/object_pool.hpp>
#include <boost/asio.hpp>

#include "Client.h"

class ConnServer {
public:
    explicit ConnServer(int port, int socketMaxConnNum = 100000, int socketMaxMsgLen = 4096, int socketTimeout = 10);

    void run();
private:
    const int DEFAULT_WRITE_BUFFER_SIZE = 4096;

    /* 长链接服务器配置 */
    int port;
    int socketMaxConnNum;
    int socketMaxMsgLen;
    int socketTimeout;
    int writeBufferSize;

    /* 客户端管理 */
    boost::asio::io_context io_context;
    boost::object_pool<Client> clientPool;
    std::unordered_map<std::string, Client> clients;
    uint64_t onlineUserConnNum;
};

using ConnServerPtr = std::shared_ptr<ConnServer>;

class ConnServerFactory {
public:
    static ConnServerPtr getConnServer(int port,
                                       int socketMaxConnNum = 100000,
                                       int socketMaxMsgLen = 4096,
                                       int socketTimeout = 10) {
        return std::make_shared<ConnServer>(port, socketMaxConnNum, socketMaxMsgLen, socketTimeout);
    }
};


#endif //IMSERVER_CONNSERVER_H
