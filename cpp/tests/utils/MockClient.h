#pragma once

#include "Client.h"

struct MockClient : public Client {
    std::string recv, sent;
    bool connected = true;

    std::string read() override;
    bool send(const std::string &data) override;
};
