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
    explicit UserInfo(const int t_userID) : userID(t_userID) {}

    [[nodiscard]] int getUserID() const {
        return userID;
    }

    void setUserID(const int user_id) {
        userID = user_id;
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

    [[nodiscard]] bool isAdmin() const {
        return userID == ADMIN_ID;
    }

private:
    static constexpr int ADMIN_ID = 1;

    int userID;
    std::string nickName;
    std::string secret;
    std::string faceURL;    // 头像地址
    std::string ex;         // 其他信息
};

#endif //USERINFO_H
