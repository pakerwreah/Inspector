//
// Created by Paker on 2020-08-23.
//

#ifndef INSPECTOR_REQUEST_H
#define INSPECTOR_REQUEST_H

#include "Client.h"
#include "Http.h"
#include <memory>

struct Request {
    std::shared_ptr<Client> client;
    Headers headers;
    std::string method, path, body;

    Request() = default;
    Request(std::shared_ptr<Client> client);
    Request(const std::string &method, const std::string &path, const std::string &body = "");
    Request(const std::string &method, const std::string &path, std::shared_ptr<Client> client, const Headers &headers);

    operator std::string() const;

    bool parse(const std::string &plain);
};

#endif //INSPECTOR_REQUEST_H
