//
// Created by Paker on 24/08/20.
//

#include "SocketClient.h"
#include <sstream>

using namespace std;

SocketClient::SocketClient(unique_ptr<Socket> socket) {
    this->socket = std::move(socket);
}

string SocketClient::read() {
    ostringstream os;
    string buf;
    while (socket->recv(buf, timeval{0, 10000})) {
        os << buf;
    }
    return os.str();
}

bool SocketClient::send(const string &data) {
    return socket->send(data);
}