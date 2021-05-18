//
// Created by Paker on 13/09/20.
//

#pragma once

#include "Client.h"
#include "Request.h"
#include "Response.h"

class WebSocket {
private:
    std::shared_ptr<Client> client;

public:
    explicit WebSocket(std::shared_ptr<Client> client);

    bool send(const std::string &data, bool binary) const;
    static Response handshake(const Request &request);
    static std::string pack(const std::string &msg, bool binary);
};
