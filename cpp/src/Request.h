//
// Created by Paker on 2020-08-23.
//

#pragma once

#include "Client.h"
#include "Http.h"
#include <memory>

struct Request {
    std::shared_ptr<Client> client;
    Headers headers;
    std::string method, path, body;

    std::string str() const;

    bool parse(const std::string &plain);
};
