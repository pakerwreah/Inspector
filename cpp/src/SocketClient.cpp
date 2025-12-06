//
// Created by Paker on 24/08/20.
//

#include "SocketClient.h"
#include <sstream>

SocketClient::SocketClient(std::unique_ptr<Socket> socket) : socket(std::move(socket)) {}

std::string SocketClient::read() const {
    return read({0, 10000});
}

bool SocketClient::send(const std::string &data) const {
    return send(data, {3, 0});
}

std::string SocketClient::read(const timeval timeout) const {
    std::ostringstream os;
    std::string buf;
    while (socket->recv(buf, timeout) > 0) {
        os << buf;
    }
    return os.str();
}

bool SocketClient::send(const std::string &data, const timeval timeout) const {
    return socket->send(data, timeout);
}
