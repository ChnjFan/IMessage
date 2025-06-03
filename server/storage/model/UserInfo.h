//
// Created by Fan on 25-6-3.
//

#ifndef USERINFO_H
#define USERINFO_H

#include <string>
#include <utility>

/**
 * @brief 用户模型
 * @details 定义用户信息模型，服务端所有用户信息均使用该类
 */
class UserInfo {
public:
    explicit UserInfo(std::string t_userID) : userID(std::move(t_userID)) {}

    [[nodiscard]] std::string getUserID() const {
        return userID;
    }

    void setUserID(std::string user_id) {
        userID = std::move(user_id);
    }

    [[nodiscard]] std::string getNickName() const {
        return nickName;
    }

    void setNickName(std::string nick_name) {
        nickName = std::move(nick_name);
    }

    [[nodiscard]] std::string getSecret() const {
        return secret;
    }

    void setSecret(std::string secret) {
        this->secret = std::move(secret);
    }

    [[nodiscard]] std::string getFaceURL() const {
        return faceURL;
    }

    void setFaceURL(std::string face_url) {
        faceURL = std::move(face_url);
    }

    [[nodiscard]] std::string getEx() const {
        return ex;
    }

    void setEx(std::string ex) {
        this->ex = std::move(ex);
    }

private:
    std::string userID;
    std::string nickName;
    std::string secret;
    std::string faceURL;    // 头像地址
    std::string ex;         // 其他信息
};

#endif //USERINFO_H
