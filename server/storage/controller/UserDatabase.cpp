//
// Created by Fan on 25-6-3.
//

#include "UserDatabase.h"

UserDatabase::UserDatabase() {
    try {
        driver = sql::mysql::get_driver_instance();
        dbConn.reset(driver->connect("tcp://127.0.0.1:3306", "IMessage", "IMessage123"));
        dbConn->setSchema("UserDatabase");
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    }
}
