//
// Created by Fan on 25-6-3.
//

#include <mysql/jdbc.h>
#include "UserDatabase.h"
#include "Exception.h"

UserDatabase::UserDatabase(const DB_CONFIG *config): dbConnPool() {
    if (config == nullptr) {
        throw Exception("UserDatabase config fail");
    }

    dbConnPool.init(USER_DATABASE_NAME, config->address, config->username, config->password, config->poolSize);
}

UserInfo* UserDatabase::find(const int userID) {
    try {
        const DBConnPool::DB_CONNECTION_PTR conn = dbConnPool.getConnection();
        // 预编译语句防止 SQL 注入
        const std::unique_ptr<sql::PreparedStatement> state(
            conn->prepareStatement("SELECT * FROM UserInfo WHERE userID = ?"));
        state->setInt(1, userID);

        if (const std::unique_ptr<sql::ResultSet> res(state->executeQuery()); res->next()) {
            const auto userInfo = new UserInfo(userID);
            userInfo->setNickName(res->getString("nickName"));
            userInfo->setSecret(res->getString("secret"));
            userInfo->setFaceURL(res->getString("faceURL"));
            userInfo->setEx(res->getString("ex"));
            return userInfo;
        }
    } catch (...) {
        return nullptr;
    }

    return nullptr;
}

std::vector<UserInfo *> UserDatabase::find(const std::vector<int> &userIDs) {
    std::vector<UserInfo *> users;
    try {
        const DBConnPool::DB_CONNECTION_PTR conn = dbConnPool.getConnection();
        //TODO:查询DB
    } catch (...) {
        return {};
    }
}

std::vector<UserInfo *> UserDatabase::findByNickName(const std::string &nickName) {
    std::vector<UserInfo *> users;
    try {
        const DBConnPool::DB_CONNECTION_PTR conn = dbConnPool.getConnection();
        const std::unique_ptr<sql::PreparedStatement> state(
            conn->prepareStatement("SELECT * FROM UserInfo WHERE nickName = ?"));
        state->setString(1, nickName);

        const std::unique_ptr<sql::ResultSet> res(state->executeQuery());
        while (res->next()) {
            const int userID = res->getInt("userID");
            const auto userInfo = new UserInfo(userID);
            userInfo->setNickName(res->getString("nickName"));
            userInfo->setSecret(res->getString("secret"));
            userInfo->setFaceURL(res->getString("faceURL"));
            userInfo->setEx(res->getString("ex"));
            users.push_back(userInfo);
        }
        return users;
    } catch (...) {
        for (const auto user : users) {
            delete user;
        }
        return {};
    }
}

bool UserDatabase::create(const UserInfo *info) {
    if (nullptr == info) {
        return false;
    }

    try {
        const DBConnPool::DB_CONNECTION_PTR conn = dbConnPool.getConnection();
        const std::unique_ptr<sql::PreparedStatement> state(
            conn->prepareStatement("INSERT INTO UserInfo (userID, nickName, faceURL, ex, secret) "
                "VALUES (?, ?, ?, ?, ?)"));
        state->setInt(1, info->getUserID());
        state->setString(2, info->getNickName());
        state->setString(3, info->getFaceURL());
        state->setString(4, info->getEx());
        state->setString(5, info->getSecret());
        const std::unique_ptr<sql::ResultSet> res(state->executeQuery());
        return res->next();
    } catch (...) {
        return false;
    }
}

bool UserDatabase::updateByMap(const int userID, std::unordered_map<std::string, USER_INFO_VALUE> &args) {
    try {
        const DBConnPool::DB_CONNECTION_PTR conn = dbConnPool.getConnection();

        // 动态构建 SQL
        std::ostringstream oss;
        oss << "UPDATE UserInfo SET ";
        int paraIndex = 0;
        for (const auto& [key, val] : args) {
            if (paraIndex++ > 0) oss << ", ";
            oss << key << " = ?";
        }
        oss << " WHERE userID = ?";

        const std::unique_ptr<sql::PreparedStatement> state(conn->prepareStatement(oss.str()));
        paraIndex = 1;
        for (const auto& [key, val] : args) {
            if (std::holds_alternative<int>(val)) {
                state->setInt(paraIndex++, std::get<int>(val));
            }
            else if (std::holds_alternative<std::string>(val)) {
                state->setString(paraIndex++, std::get<std::string>(val));
            }
        }
        state->setInt(paraIndex, userID);
        const std::unique_ptr<sql::ResultSet> res(state->executeQuery());
        return res->next();
    } catch (...) {
        return false;
    }
}

bool UserDatabase::isExist(const int userID) {
    try {
        const DBConnPool::DB_CONNECTION_PTR conn = dbConnPool.getConnection();
        const std::unique_ptr<sql::PreparedStatement> state(
        conn->prepareStatement("SELECT 1 FROM UserInfo WHERE userID = ? LIMIT 1"));
        state->setInt(1, userID);

        const std::unique_ptr<sql::ResultSet> res(state->executeQuery());
        return res->next();
    } catch (...) {
        return false;
    }
}
