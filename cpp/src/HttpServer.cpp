//
// Created by Paker on 2019-10-23.
//

#include <sstream>
#include <thread>
#include "HttpServer.h"
#include "Socket.h"
#include "picohttpparser.h"

Request::Request(const string &plain) {
    const char *method, *path;
    size_t path_len, method_len;
    int minor_version;
    size_t num_headers = 50;
    struct phr_header headers[num_headers];
    const char *buffer = plain.c_str();

    int body_start = phr_parse_request(
            buffer, plain.length(),
            &method, &method_len,
            &path, &path_len,
            &minor_version,
            headers, &num_headers, 0);

    if (method_len) {
        this->method = string(method).substr(0, method_len);
        this->path = string(path).substr(0, path_len);

        for (int i = 0; i < num_headers; i++) {
            auto header = headers[i];
            auto name = string(header.name).substr(0, header.name_len);
            auto value = string(header.value).substr(0, header.value_len);
            this->headers[name] = value;
        }

        this->body = plain.substr(static_cast<unsigned long>(body_start));
    }
}

bool Request::valid() {
    return !method.empty();
}

Response::Response(string body, int code) {
    this->body = body;
    this->code = code;
}

Response::operator string() {
    ostringstream resp;
    resp << "HTTP/1.1 " << code << "\n"
         << "Content-Type: " << content_type << " charset=utf-8\n"
         << "Content-Length: " << body.length() << "\n";

    for (auto header : headers) {
        resp << header.first << ": " << header.second << "\n";
    }

    resp << "\n" << body;
    return resp.str();
}

void HttpServer::request(string method, string path, Handler handler) {
    routes[method][path] = handler;
}

void HttpServer::get(string path, Handler handler) {
    request("GET", path, handler);
}

void HttpServer::post(string path, Handler handler) {
    request("POST", path, handler);
}

void HttpServer::put(string path, Handler handler) {
    request("PUT", path, handler);
}

void HttpServer::stop() {
    _stop = true;
}

thread *HttpServer::start(int port) {
    _stop = false;
    return new thread([this, port] {
        while (!_stop) {
            Socket server;
            server.create();
            server.bind(port);
            server.listen();

            Socket *client;
            while (server.accept(client)) {
                string plain, buf;

                while (client->recv(buf, timeval{0, 100000})) {
                    plain += buf;
                }

                Request request(plain);

                if (request.valid()) {
                    try {
                        auto handler = routes.at(request.method).at(request.path);

                        auto response = handler(request);

                        client->send(response);

                    } catch (out_of_range ex) {
                        client->send(Response::NotFound());
                    }
                }

                delete client;
            }
        }
    });
}
