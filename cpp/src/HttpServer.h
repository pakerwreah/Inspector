//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_HTTPSERVER_H
#define INSPECTOR_HTTPSERVER_H

#include <map>
#include <string>
#include <memory>
#include <thread>
#include <functional>

#include "Client.h"
#include "Router.h"

class HttpServer {
    bool _stop;

protected:
    void process(std::shared_ptr<Client> client) const;

public:
    Router router;

    virtual ~HttpServer();

    void stop();

    std::thread *start(int port);
};


#endif //INSPECTOR_HTTPSERVER_H
