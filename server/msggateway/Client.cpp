//
// Created by fan on 25-4-17.
//

#include "Client.h"

Client::Client(SessionPtr session) : conn(session) {
}

int Client::getUserID() const {
    return conn->getUserID();
}

void Client::close() {
    conn->close();
}

bool Client::down() const {
    return conn->getState() == SessionState::SESSION_DELETED;
}

ClientPtr Client::constructor(SessionPtr session) {
    return std::make_shared<Client>(session);
}

void Client::setType(CLIENT_TYPE t_type) {
    type = t_type;
}
