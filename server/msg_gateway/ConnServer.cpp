//
// Created by fan on 25-4-13.
//

#include "ConnServer.h"

ConnServer::ConnServer(int port, int socketMaxConnNum, int socketMaxMsgLen, int socketTimeout)
    : port(port)
    , socketMaxConnNum(socketMaxConnNum)
    , socketMaxMsgLen(socketMaxMsgLen)
    , socketTimeout(socketTimeout)
    , writeBufferSize(DEFAULT_WRITE_BUFFER_SIZE)
    , onlineUserConnNum(0) {

}

void ConnServer::run() {

}
