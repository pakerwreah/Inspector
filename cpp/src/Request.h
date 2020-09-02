//
// Created by Paker on 2020-08-23.
//

#ifndef INSPECTOR_REQUEST_H
#define INSPECTOR_REQUEST_H

#include <map>
#include <string>
#include <memory>

#include "Client.h"
#include "Http.h"

struct Request {
    std::shared_ptr<Client> client;
    Headers headers;
    std::string method, path, body;

    Request(std::shared_ptr<Client> client = nullptr);

    bool parse(const std::string &plain);
};

#endif //INSPECTOR_REQUEST_H
