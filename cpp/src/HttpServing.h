//
// Created by Paker on 22/09/20.
//

#ifndef INSPECTOR_HTTPSERVING_H
#define INSPECTOR_HTTPSERVING_H

#include "Client.h"
#include <memory>

struct HttpServing {
    virtual void process(std::shared_ptr<Client> client) const = 0;
};

#endif //INSPECTOR_HTTPSERVING_H
