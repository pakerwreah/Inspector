//
// Created by Paker on 24/08/20.
//

#include "SocketClient.h"
#include <sstream>

using namespace std;

SocketClient::SocketClient(unique_ptr<Socket> socket) : socket(std::move(socket)) {}

string SocketClient::read() const {
    return read({0, 10000});
}

bool SocketClient::send(const string &data) const {
    return send(data, {3, 0});
}

string SocketClient::read(const timeval timeout) const {
    ostringstream os;
    string buf;
    while (socket->recv(buf, timeout) > 0) {
        os << buf;
    }
    return os.str();
}

bool SocketClient::send(const string &data, const timeval timeout) const {
    return socket->send(data, timeout);
}
