//
// Created by Fan on 25-6-20.
//

#ifndef RETURNCODE_H
#define RETURNCODE_H

enum class SERVER_RETURN_CODE {
    SUCCESS = 0,
    SERVER_INNER_ERROR = 1000,      // 服务器内部错误
    SESSION_TIMEOUT = 1001,         // 会话超时
    SESSION_OVERRUN = 1002,         // 会话个数超过最大限制

    SERVER_MESSAGE_ERROR = 2000,    // 消息错误
    REQUEST_ERROR = 2001,           // 请求错误
    AUTH_ERROR = 2002,              // 认证失败
};

#endif //RETURNCODE_H
