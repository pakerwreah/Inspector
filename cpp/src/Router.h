//
// Created by Paker on 24/08/20.
//

#pragma once

#include "Request.h"
#include "Response.h"

typedef std::function<Response(const Request &, const Params &)> Handler;

class Router {
private:
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
