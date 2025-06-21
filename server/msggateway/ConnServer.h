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

#include "Client.h"
#include "ClientManager.h"
#include "ConfigManager.h"
#include "Session.h"
#include "MsgGatewayUtil.h"

using namespace boost::asio;
using ip::tcp;

class ConnServer : public std::enable_shared_from_this<ConnServer> {
public:
    explicit ConnServer(io_context &io, std::shared_ptr<ConfigManager> &configMgr, int port, int socketMaxConnNum = 100000,
            int socketMaxMsgLen = 4096, int socketTimeout = 10);

    /**
     * @brief 运行服务器
     * @note  消息网关连接服务器，用于接收客户端连接
     */
    void run();

    void handleRequest(const SessionPtr& session, const MessagePtr &message);

private:
    void startDetectionLoop();
    void unauthSessionDetector();

    /**
     * @brief hello 报文处理
     * @param session 会话终端
     * @param message 请求消息
     * @note 客户端成功建链后立即发送 hello 报文，会话状态转换为 READY 可以接收客户端登录注册消息
     */
    void helloRequest(const SessionPtr & session, const MessagePtr & message);
    /**
     * @brief 新会话请求消息处理
     * @param session 会话终端
     * @param message 请求消息
     * @note 新会话请求消息只能是终端用户认证或者用户注册消息
     */
    void newSessionRequest(const SessionPtr & session, const MessagePtr & message);

    const int DEFAULT_WRITE_BUFFER_SIZE = 4096;

    std::mutex mutex;

    /* 长链接服务器配置 */
    std::shared_ptr<ConfigManager> configMgr;

    int port;
    int socketMaxConnNum;
    int socketMaxMsgLen;
    int socketTimeout;
    int writeBufferSize;

    /* 客户端管理 */
    int onlineSessionNum;
    tcp::acceptor acceptor;
    std::unordered_map<std::string, ClientPtr> clients;
    std::list<SessionPtr> unauthorizedSessions;
    steady_timer taskTimer;

    /* rpc 服务客户端 */
    ClientManager clientManager;
};

using ConnServerPtr = std::shared_ptr<ConnServer>;

class ConnServerFactory {
public:
    static ConnServerPtr getConnServer(boost::asio::io_context &io,
            std::shared_ptr<ConfigManager> &configMgr,
            int port,
            int socketMaxConnNum = 100000,
            int socketMaxMsgLen = 4096,
            int socketTimeout = 10) {
        return std::make_shared<ConnServer>(io, configMgr, port, socketMaxConnNum, socketMaxMsgLen, socketTimeout);
    }
};


#endif //IMSERVER_CONNSERVER_H
