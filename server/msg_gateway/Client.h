//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_CLIENT_H
#define IMSERVER_CLIENT_H

#include <memory>

#include "Session.h"

class Client;
typedef std::shared_ptr<Client> ClientPtr;

class Client {
public:
    explicit Client(SessionPtr session);

    std::string getUserID() const;

    static ClientPtr constructor(SessionPtr session);

private:
    /* 用户会话 */
    SessionPtr  conn;

};


#endif //IMSERVER_CLIENT_H
