//
// Created by fan on 25-4-17.
//

#ifndef IMSERVER_CLIENT_H
#define IMSERVER_CLIENT_H

#include <memory>

#include "TCPSession.h"

enum class CLIENT_TYPE {
    ADMIN = 1,
    NORMAL = 2,
};

class Client;
typedef std::shared_ptr<Client> ClientPtr;

class Client {
public:
    explicit Client(SessionPtr session);

    int getUserID() const;

    void close();

    bool down() const;

    static ClientPtr constructor(SessionPtr session);

    void setType(CLIENT_TYPE t_type);

private:
    /* 用户会话 */
    SessionPtr  conn;
    CLIENT_TYPE type;
};


#endif //IMSERVER_CLIENT_H
