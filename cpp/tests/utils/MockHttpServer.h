#ifndef INSPECTOR_MOCKHTTPSERVER_H
#define INSPECTOR_MOCKHTTPSERVER_H

#include "HttpServer.h"

struct MockHttpServer : public HttpServer {
    std::function<void(std::shared_ptr<Client>)> processor;

    void process(std::shared_ptr<Client> client) const override;
};


#endif //INSPECTOR_MOCKHTTPSERVER_H
