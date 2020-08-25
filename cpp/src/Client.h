//
// Created by Paker on 24/08/20.
//

#ifndef INSPECTOR_TESTS_CLIENT_H
#define INSPECTOR_TESTS_CLIENT_H

#include <string>

struct Client {
    virtual std::string read() = 0;
    virtual bool send(const std::string &data) = 0;
};

#endif //INSPECTOR_TESTS_CLIENT_H
