//
// Created by Paker on 22/09/20.
//

#pragma once

#include "Client.h"
#include <memory>

struct HttpServing {
    virtual void process(std::shared_ptr<Client> client) const = 0;
};
