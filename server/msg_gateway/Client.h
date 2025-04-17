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

private:
    /* 用户 socket 链接 */
    std::unique_ptr<Session> conn;
    /* 链接信息 */
    std::string token;
    std::string userID;
    int platformID;

};


#endif //IMSERVER_CLIENT_H
