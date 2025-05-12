//
// Created by Fan on 25-5-12.
//

#include "Message.h"

bool Message::decode()  {
    // TODO:消息解密
    memcpy(&checkSum, body() + bodyLen, MESSAGE_DEFAULT_HEADER_SIZE);
    boost::endian::big_to_native_inplace(checkSum);

    // 校验消息 crc
    boost::crc_32_type crc32;
    crc32.process_bytes(body(), bodyLen);
    if (checkSum != static_cast<int32_t>(crc32.checksum())) {
        return false;
    }

    method = std::string(body());
    return true;
}
