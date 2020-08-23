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

#include "Request.h"
#include "Response.h"

typedef std::map<std::string, std::string> Params;
typedef std::function<Response(const Request &, const Params &)> Handler;

class HttpServer {
    bool _stop = false;

    // method: { path: handler }
    std::map<std::string, std::map<std::string, Handler>> routes;

    void request(const std::string &method, const std::string &path, Handler handler);

    Handler find_handler(const Request &request, Params *params) const;

    void process(std::shared_ptr<Socket> client) const;

public:
    virtual ~HttpServer();

    void get(const std::string &path, Handler handler);
    void post(const std::string &path, Handler handler);
    void put(const std::string &path, Handler handler);
    void request(const std::string &path, Handler handler);

    void stop();

    std::thread *start(int port);
};


#endif //INSPECTOR_HTTPSERVER_H
