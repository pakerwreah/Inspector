//
// Created by Paker on 24/08/20.
//

#ifndef INSPECTOR_ROUTER_H
#define INSPECTOR_ROUTER_H

#include <map>
#include <string>
#include "Request.h"
#include "Response.h"

typedef std::function<Response(const Request &, const Params &)> Handler;

class Router {
    // method: { path: handler }
    std::map<std::string, std::map<std::string, Handler>> routes;

public:
    Response handle(const Request &request) const;

    void route(const std::string &method, const std::string &path, Handler handler);
    // convenience methods
    void get(const std::string &path, Handler handler);
    void post(const std::string &path, Handler handler);
    void put(const std::string &path, Handler handler);
};

#endif //INSPECTOR_ROUTER_H
