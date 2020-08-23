//
// Created by Paker on 2020-08-23.
//

#ifndef INSPECTOR_REQUEST_H
#define INSPECTOR_REQUEST_H

#include <map>
#include <string>
#include <memory>

#include "Socket.h"

struct Request {
    std::shared_ptr<Socket> socket;
    std::map<std::string, std::string> headers;
    std::string method, path, body;

    Request(const std::string &plain, std::shared_ptr<Socket> client);

    bool is_valid();
};

#endif //INSPECTOR_REQUEST_H
