//
// Created by fan on 25-4-13.
//

#ifndef IMSERVER_CONNSERVER_H
#define IMSERVER_CONNSERVER_H

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <boost/pool/object_pool.hpp>
#include <boost/asio.hpp>

#include "server.h"
#include "Client.h"
#include "Session.h"

using namespace boost::asio;
using ip::tcp;

class ConnServer : public std::enable_shared_from_this<ConnServer> {
public:
    explicit ConnServer(io_context &io, int port, int socketMaxConnNum = 100000,
            int socketMaxMsgLen = 4096, int socketTimeout = 10);

    void run();

    void deleteClient(const std::string &token);

private:
    void detectionTasks();
    void handleUnauthenSession();
    void handleClients();

    void handler();
    void handleNewConnection(const boost::system::error_code &ec, SessionPtr &session);

    void error(const SessionPtr &session, SERVER_RETURN_CODE code, std::string error);

    const int DEFAULT_WRITE_BUFFER_SIZE = 4096;

    /* 长链接服务器配置 */
    int port;
    int socketMaxConnNum;
    int socketMaxMsgLen;
    int socketTimeout;
    int writeBufferSize;

    /* 客户端管理 */
    int onlineUserConnNum;
    tcp::acceptor acceptor;
    boost::object_pool<Client> clientPool;
    std::unordered_map<std::string, ClientPtr> clients;
    std::list<SessionPtr> unauthorizedSessions;
    steady_timer taskTimer;
};

using ConnServerPtr = std::shared_ptr<ConnServer>;

class ConnServerFactory {
public:
    static ConnServerPtr getConnServer(boost::asio::io_context &io,
            int port,
            int socketMaxConnNum = 100000,
            int socketMaxMsgLen = 4096,
            int socketTimeout = 10) {
        return std::make_shared<ConnServer>(io, port, socketMaxConnNum, socketMaxMsgLen, socketTimeout);
    }
};


#endif //IMSERVER_CONNSERVER_H
