//
// Created by Fan on 25-6-3.
//

#ifndef USERDATABASE_H
#define USERDATABASE_H

#include "ConfigManager.h"
#include "DBConnPool.h"
#include "model/UserInfo.h"

class UserDatabase {
public:
    constexpr std::string USER_DATABASE_NAME = "UserDatabase";
    using USER_INFO_VALUE = std::variant<int, std::string>;

    explicit UserDatabase(const DB_CONFIG *config);

    UserInfo* find(int userID);
    std::vector<UserInfo*> find(const std::vector<int>& userIDs);
    std::vector<UserInfo*> findByNickName(const std::string& nickName);
    bool create(const UserInfo *info);
    bool updateByMap(int userID, std::unordered_map<std::string, USER_INFO_VALUE>& args);
    bool isExist(int userID);

private:
    DBConnPool dbConnPool;
};



#endif //USERDATABASE_H
