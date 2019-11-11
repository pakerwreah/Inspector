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
    Request(const string &plain, Socket *pSocket);

    shared_ptr<Socket> socket;
    map<string, string> headers, params;
    string method, path, body;

    bool valid();
};

struct Response {
    Response(json data = nullptr, int code = 200, string content_type = ContentType::JSON);

    operator string();

    map<string, string> headers;
    int code;
    string content_type;
    string body;

    static Response NotFound() {
        return Response("Route not found", 404);
    }
};

typedef function<Response(const Request &)> Handler;

class HttpServer {
    bool _stop = false;

    map<string, map<string, Handler>> routes;

    void request(string method, string path, Handler handler);

    Handler find_route(Request &request);

public:
    void get(string path, Handler handler);

    void post(string path, Handler handler);

    void put(string path, Handler handler);

    void stop();

    thread *start(int port);
};


#endif //INSPECTOR_HTTPSERVER_H
