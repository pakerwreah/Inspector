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

    Request() = default;
    explicit Request(std::shared_ptr<Client> client);
    Request(const std::string &method, const std::string &path, const std::string &body = "");
    Request(const std::string &method, const std::string &path, std::shared_ptr<Client> client, const Headers &headers);

    operator std::string() const;

    bool parse(const std::string &plain);
};
