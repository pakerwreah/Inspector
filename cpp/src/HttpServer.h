//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_HTTPSERVER_H
#define INSPECTOR_HTTPSERVER_H

#include <map>
#include <string>
#include <thread>
#include <functional>
#include "libs/json.hpp"

using json = nlohmann::json;

using namespace std;

class Socket;

namespace ContentType {
    const string HTML = "text/html";
    const string JSON = "application/json";
}

struct Request {
    shared_ptr<Socket> socket;
    map<string, string> headers;
    string method, path, body;

    Request(const string &plain, shared_ptr<Socket> client);

    bool valid();
};

struct Response {
    Response(const json &data = nullptr, int code = 200, const string &content_type = ContentType::JSON);

    operator string();

    map<string, string> headers;
    int code;
    string content_type;
    string body;

    static Response NotFound() {
        return Response("Route not found", 404);
    }

    static Response InternalError() {
        return Response("Internal Error", 500);
    }
};

typedef map<string, string> Params;
typedef function<Response(const Request &, const Params &)> Handler;

class HttpServer {
    bool _stop = false;

    map<string, map<string, Handler>> routes;

    void request(const string &method, const string &path, Handler handler);

    Handler find_handler(const Request &request, Params *params) const;

    void process(shared_ptr<Socket> client) const;

public:
    void get(const string &path, Handler handler);

    void post(const string &path, Handler handler);

    void put(const string &path, Handler handler);

    void stop();

    thread *start(int port);
};


#endif //INSPECTOR_HTTPSERVER_H
