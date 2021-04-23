//
// Created by Paker on 24/08/20.
//

#ifndef INSPECTOR_SOCKETCLIENT_H
#define INSPECTOR_SOCKETCLIENT_H

#include "Client.h"
#include "Socket.h"
#include <memory>

class SocketClient : public Client {
private:
    std::unique_ptr<Socket> socket;

public:
    SocketClient(std::unique_ptr<Socket> socket);
    virtual ~SocketClient() = default;

    std::string read() override;
    std::string read(const timeval &timeout) const;
    bool send(const std::string &data) override;
};

#endif //INSPECTOR_SOCKETCLIENT_H
