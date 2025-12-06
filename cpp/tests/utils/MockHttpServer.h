#pragma once

#include "HttpServer.h"

struct MockHttpServer : HttpServer {
    std::function<void(std::shared_ptr<Client>)> processor;

    void process(std::shared_ptr<Client> client) const override;
};
