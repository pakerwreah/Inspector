//
// Created by Paker on 24/08/20.
//

#ifndef INSPECTOR_ROUTER_H
#define INSPECTOR_ROUTER_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Request.h"

typedef std::map<std::string, std::string> Params;

struct Route {
    std::string method, path;

    bool operator==(const Route &other) const {
        return method == other.method && path == other.path;
    }
};

namespace RouteParser {
    Params decode(const std::string &urlencoded);
    std::pair<Route, Params> parse(const std::vector<Route> &routes, const Request &request);
}

template<typename Handler>
class Router {
    struct RouteHasher {
        static constexpr auto hash = std::hash<std::string>();

        size_t operator()(const Route &route) const {
            return hash(route.method) ^ hash(route.path);
        }
    };

    std::vector<Route> routes;
    std::unordered_map<Route, Handler, RouteHasher> handlers;

public:
    std::pair<Handler, Params> parse(const Request &request) const {
        const auto &[route, params] = RouteParser::parse(routes, request);
        return {handlers.at(route), params};
    }

    void route(const std::string &method, const std::string &path, Handler handler) {
        routes.push_back({method, path});
        handlers[{method, path}] = handler;
    }

    // convenience methods
    void any(const std::string &path, Handler handler) {
        route("*", path, handler);
    }

    void get(const std::string &path, Handler handler) {
        route("GET", path, handler);
    }

    void post(const std::string &path, Handler handler) {
        route("POST", path, handler);
    }

    void put(const std::string &path, Handler handler) {
        route("PUT", path, handler);
    }
};


#endif //INSPECTOR_ROUTER_H
