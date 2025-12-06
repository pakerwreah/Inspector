#pragma once

#include "Client.h"

struct MockClient : Client {
    std::string recv;
    mutable std::string sent;
    bool connected = true;

    std::string read() const override;
    bool send(const std::string &data) const override;
};
