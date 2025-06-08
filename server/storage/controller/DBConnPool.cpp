//
// Created by Fan on 25-6-7.
//

#include "DBConnPool.h"
#include "Exception.h"
#include "boost/json/detail/literals.hpp"

void DBConnPool::init(const std::string& database, const std::string &address, const std::string &username,
                        const std::string &password, const int t_capacity) {
    driver = sql::mysql::get_driver_instance();
    capacity = t_capacity;
    m_address = address;
    m_database = database;
    m_username = username;
    m_password = password;

    try {
        for (int i = 0; i < capacity; ++i) {
            sql::Connection *conn = driver->connect(m_database, m_username, m_password);
            if (conn == nullptr) {
                throw Exception("Failed to connect to database '" + m_address);
            }
            conn->setSchema(m_database);
            connections.push(conn);
        }
    } catch (sql::SQLException &e) {
        throw Exception("Connection fail to database: " + m_address);
    }
}

DBConnPool::DB_CONNECTION_PTR DBConnPool::getConnection() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [this] { return !connections.empty(); });

    sql::Connection *conn = connections.front();
    connections.pop();

    if (!isConnectionAlive(conn)) {
        // 连接失效，重新连接
        delete conn;
        conn = driver->connect(m_address, m_username, m_password);
        conn->setSchema(m_database);
    }

    return DB_CONNECTION_PTR(conn, [this](sql::Connection *pConn) {
        std::lock_guard<std::mutex> delLock(mutex);
        connections.push(pConn);
        condition.notify_one();
    });
}

DBConnPool::~DBConnPool() {
    while (!connections.empty()) {
        const auto conn = connections.front();
        connections.pop();
        delete conn;
    }
}

bool DBConnPool::isConnectionAlive(sql::Connection *conn) {
    try {
        return conn->isValid();
    } catch (...) {
        return false;
    }
}


