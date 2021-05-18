//
// Created by Paker on 24/08/20.
//

#pragma once

#include <string>

struct Client {
    virtual std::string read() = 0;
    virtual bool send(const std::string &data) = 0;
};
