//
// Created by Paker on 24/08/20.
//

#pragma once

#include <string>

struct Client {
    virtual std::string read() const = 0;
    virtual bool send(const std::string &data) const = 0;
};
