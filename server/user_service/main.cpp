//
// Created by Fan on 25-4-26.
//

#include "ServiceImpl.h"

int main() {
    ServiceImpl service;
    std::string ip = "127.0.0.1";
    service.run(ip, 10002);
    return 0;
}
