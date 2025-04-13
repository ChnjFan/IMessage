/**
 * @file Exception.h
 * @brief 异常管理
 * @author ChnjFan
 * @date 2024-12-04
 * @copyright Copyright (c) 2024 ChnjFan. All rights reserved.
 */

#ifndef IMSERVER_EXCEPTION_H
#define IMSERVER_EXCEPTION_H

#include <string>
#include <exception>

/**
 * @class Exception
 * @brief A custom exception class for handling errors.
 * This class extends the standard exception class to provide custom error messages.
 */
class Exception : public std::exception {
public:
    explicit Exception(const std::string what) : message(what) { }

    ~Exception() noexcept override = default;

    const char *what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

#endif //IMSERVER_EXCEPTION_H
