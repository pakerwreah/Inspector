//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_HTTPSERVER_H
#define INSPECTOR_HTTPSERVER_H

#include <map>
#include <string>
#include <functional>

using namespace std;

struct Request {
    Request(const string &plain);

    map<string, string> headers;
    string method, path, body;

    bool valid();
};

struct Response {
    Response(string body = "", int code = 200);

    operator string();

    map<string, string> headers;
    int code;
    string content_type = "text/html";
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

public:
    void get(string path, Handler handler);

    void post(string path, Handler handler);

    void put(string path, Handler handler);

    void stop();

    thread *start(int port);
};


#endif //INSPECTOR_HTTPSERVER_H
