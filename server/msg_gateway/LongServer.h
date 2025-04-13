//
// Created by fan on 25-4-13.
//

#ifndef IMSERVER_LONGSERVER_H
#define IMSERVER_LONGSERVER_H

#include <memory>

namespace MsgGateWay {

class LongServer {
public:
    explicit LongServer(int port, int socketMaxConnNum = 100000, int socketMaxMsgLen = 4096, int socketTimeout = 10);
private:
    int port;
    int socketMaxConnNum;
    int socketMaxMsgLen;
    int socketTimeout;
};

class LongServerFactory {
public:
    static std::shared_ptr<LongServer> getLongServer(int port,
                                                     int socketMaxConnNum = 100000,
                                                     int socketMaxMsgLen = 4096,
                                                     int socketTimeout = 10) {
        return std::make_shared<LongServer>(port, socketMaxConnNum, socketMaxMsgLen, socketTimeout);
    }
};

}

#endif //IMSERVER_LONGSERVER_H
