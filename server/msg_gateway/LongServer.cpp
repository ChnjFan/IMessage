//
// Created by fan on 25-4-13.
//

#include "LongServer.h"

MsgGateWay::LongServer::LongServer(int port, int socketMaxConnNum, int socketMaxMsgLen, int socketTimeout)
    : port(port)
    , socketMaxConnNum(socketMaxConnNum)
    , socketMaxMsgLen(socketMaxMsgLen)
    , socketTimeout(socketTimeout) {

}
