//
// Created by Fan on 25-4-22.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <memory>
#include <boost/crc.hpp>
#include <boost/endian/conversion.hpp>

// 消息类型定义
#define MESSAGE_REQUEST_HELLO           "session.hello"
#define MESSAGE_REQUEST_REGISTER        "user.register"
#define MESSAGE_REQUEST_AUTH            "user.auth"

class Message;
using MessagePtr = std::shared_ptr<Message>;

/**
 * Message 消息格式：
 * |+++++++++++++|+++++++++++++|+++++++++|++++++++++++++|
 * | MessageSize | RequestName | MsgBody | CRC CheckSum |
 * |+++++++++++++|+++++++++++++|+++++++++|++++++++++++++|
 * MessageSize：4字节，消息长度为 RequestName + MsgBody 长度
 * RequestName：字符串，必须 '\0' 结尾
 * MsgBody：消息内容
 * CRC CheckSum：32位 crc 校验值
 **/
class Message {
public:
    static constexpr int MESSAGE_DEFAULT_HEADER_SIZE = sizeof(int);
    static constexpr std::size_t MESSAGE_DEFAULT_BODY_SIZE = 512;

    explicit Message(const int headerSize = MESSAGE_DEFAULT_HEADER_SIZE, const std::size_t bodySize = MESSAGE_DEFAULT_BODY_SIZE)
          : headerSize(headerSize)
          , bodySize(bodySize)
          , bodyLen(0)
          , pBuffer(new char[headerSize + bodySize])
          , checkSum(0) { }

    Message(const char* body, const std::size_t size)
          : Message(MESSAGE_DEFAULT_HEADER_SIZE, size + sizeof(int32_t)) {
        bodyLen = size;
        memcpy(pBuffer, body, bodyLen);
        boost::crc_32_type crc32;
        crc32.process_bytes(pBuffer, bodyLen);
        checkSum = static_cast<int32_t>(crc32.checksum());
    }

    Message(const Message& message)
            : headerSize(message.headerSize)
            , bodySize(message.bodySize)
            , bodyLen(message.bodyLen)
            , pBuffer(new char[message.size()])
            , checkSum(message.checkSum) {
        memcpy(pBuffer, message.pBuffer, message.size());
    }

    ~Message() {
        if (nullptr != pBuffer) {
            delete[] pBuffer;
            pBuffer = nullptr;
        }
    }

    [[nodiscard]] const char* body() const {
        return pBuffer + headerSize;
    }

    char* body() {
        return pBuffer + headerSize;
    }

    [[nodiscard]] const char* header() const {
        return pBuffer;
    }

    char* header() {
        return pBuffer;
    }

    [[nodiscard]] std::size_t length() const {
        return headerSize + bodyLen;
    }

    void setBodyLength(std::size_t len) {
        bodyLen = len;
    }

    [[nodiscard]] std::size_t getBodyLen() const {
        return bodyLen;
    }

    [[nodiscard]] std::size_t size() const {
        return headerSize + bodySize;
    }

    [[nodiscard]] std::size_t getBodySize() const {
        return bodySize;
    }

    bool decode();

    char* encode() {
        boost::endian::native_to_big_inplace(bodyLen);
        memcpy(pBuffer, &bodyLen, headerSize);
        boost::endian::native_to_big_inplace(checkSum);
        memcpy(pBuffer + length(), &checkSum, sizeof(checkSum));
        return pBuffer;
    }

    std::string& getMethod() {
        return method;
    }

    void clear() {
        bodyLen = 0;
        method.clear();
        checkSum = 0;
    }

private:

    int headerSize;
    std::size_t bodySize;
    std::size_t bodyLen;

    std::string method;
    char *pBuffer;

    int32_t checkSum;
};

typedef std::shared_ptr<Message> MessagePtr;

#endif //MESSAGE_H
