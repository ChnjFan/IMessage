//
// Created by Fan on 25-5-12.
//

#include <boost/json.hpp>
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

std::string Message::responseFormat(SERVER_RETURN_CODE code, const std::string &message, const std::string &detail,
                                    const std::unordered_map<std::string, std::string> &data) {
    boost::json::object obj;

    obj["code"] = static_cast<int>(code);
    obj["message"] = message;
    if (!detail.empty())
        obj["detail"] = detail;

    if (!data.empty()) {
        for (const auto &[item, value] : data) {
            obj["data"].as_object()[item] = value;
        }
    }

    return boost::json::serialize(obj);
}
