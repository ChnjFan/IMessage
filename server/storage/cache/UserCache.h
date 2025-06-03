//
// Created by Fan on 25-6-3.
//

#ifndef USERCACHE_H
#define USERCACHE_H

#include <unordered_map>

#include "model/UserInfo.h"

class UserCache {
public:
    static constexpr int USER_CACHE_DEFAULT_SIZE = 1024;

    explicit UserCache(int capacity = USER_CACHE_DEFAULT_SIZE);

    void
private:
    std::unordered_map<std::string, UserInfo> cache;
};



#endif //USERCACHE_H
