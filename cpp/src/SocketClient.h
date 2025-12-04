//
// Created by Paker on 24/08/20.
//

#pragma once

#include "Client.h"
#include "Socket.h"
#include <memory>

class SocketClient : public Client {
private:
    std::unique_ptr<Socket> socket;

public:
    explicit SocketClient(std::unique_ptr<Socket> socket);

    std::string read() const override;
    bool send(const std::string &data) const override;

    std::string read(timeval timeout) const;
    bool send(const std::string &data, timeval timeout) const;
};
