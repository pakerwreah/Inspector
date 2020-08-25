//
// Created by Paker on 24/08/20.
//

#ifndef INSPECTOR_SOCKETCLIENT_H
#define INSPECTOR_SOCKETCLIENT_H

#include "Client.h"
#include "Socket.h"

class SocketClient : public Client {
    std::unique_ptr<Socket> socket;
public:
    SocketClient(std::unique_ptr<Socket> socket);

    std::string read() override;
    bool send(const std::string &data) override;
};


#endif //INSPECTOR_SOCKETCLIENT_H
