//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_CLIENT_H
#define IMSERVER_CLIENT_H

#include <string>
#include <memory>

#include "Session.h"

class Client {
public:
    Client() = default;
    Client(const Client&) = default;
    explicit Client(SessionPtr session);

private:
    /* 用户会话链接 */
    SessionPtr  conn;
    /* 链接信息 */
    std::string token;
    std::string userID;
    int platformID;

};


#endif //IMSERVER_CLIENT_H
