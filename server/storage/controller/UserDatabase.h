//
// Created by Fan on 25-6-3.
//

#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <mysql/jdbc.h>

#include "model/UserInfo.h"

class UserDatabase {
public:
    UserDatabase();
private:
    sql::mysql::MySQL_Driver *driver;
    std::unique_ptr<sql::Connection> dbConn;
};



#endif //USERDATABASE_H
