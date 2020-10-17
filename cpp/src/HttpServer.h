//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_HTTPSERVER_H
#define INSPECTOR_HTTPSERVER_H

#include <memory>
#include <thread>

#include "Client.h"
#include "HttpServing.h"
#include "Router.h"
#include "Socket.h"

class HttpServer : public HttpServing {
private:
    bool _stop;
    bool _listening;
    int _error;
    std::chrono::nanoseconds interval;

protected:
    Socket server;

    void process(std::shared_ptr<Client> client) const override;

public:
    Router router;

    HttpServer();
    virtual ~HttpServer();

    int error() const;
    bool stop();
    bool listening() const;
    std::thread *start(int port);
    void setReconnectInterval(std::chrono::nanoseconds interval);
};

#endif //INSPECTOR_HTTPSERVER_H
