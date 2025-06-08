//
// Created by Fan on 25-6-7.
//

#ifndef DBCONNPOOL_H
#define DBCONNPOOL_H

#include <string>
#include <mysql/jdbc.h>

class DBConnPool {
public:
    using DB_CONNECTION_PTR = std::shared_ptr<sql::Connection>;
    DBConnPool() = default;
    ~DBConnPool();

    void init(const std::string& database, const std::string& address, const std::string& username, const std::string& password, int t_capacity);

    DB_CONNECTION_PTR getConnection();
private:
    static bool isConnectionAlive(sql::Connection *conn);

    std::string m_database;
    std::string m_address;
    std::string m_username;
    std::string m_password;
    int capacity;
    sql::mysql::MySQL_Driver* driver = nullptr;
    std::queue<sql::Connection*> connections;
    std::mutex mutex;
    std::condition_variable condition;
};



#endif //DBCONNPOOL_H
