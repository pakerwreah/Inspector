//
// Created by Paker on 2019-10-23.
//

#include <sstream>
#include <thread>
#include "HttpServer.h"
#include "Socket.h"
#include "picohttpparser.h"
#include "util.h"
#include "compress.hpp"

using namespace util;

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

Response::Response(json data, int code, string content_type) {
    string resp;
    if (data.is_null()) {
        resp = "";
    } else if (data.is_string()) {
        if (content_type == ContentType::JSON) {
            resp = json{{"msg", data}}.dump();
        } else {
            resp = data.get<string>();
        }
    } else {
        resp = data.dump();
    }
    this->content_type = content_type;
    this->body = resp;
    this->code = code;
}

Response::operator string() {
    ostringstream resp;
    resp << "HTTP/1.1 " << code << "\n"
         << "Content-Type: " << content_type << " charset=utf-8\n"
         << "Content-Length: " << body.length() << "\n"
         << "Access-Control-Allow-Origin: *\n";

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

                while (client->recv(buf, timeval{0, 8000})) {
                    plain += buf;
                }

                Request request(plain);

                if (request.valid()) {
                    try {
                        if (request.method == "OPTIONS") {
                            Response response;
                            response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT";
                            client->send(response);
                        } else {
                            Handler handler = find_route(request);

                            auto response = handler(request);

                            if (request.headers["Accept-Encoding"].find("gzip") >= 0) {
                                response.body = gzip::compress(response.body.data(), response.body.size());
                                response.headers["Content-Encoding"] = "gzip";
                            }

                            client->send(response);
                        }
                    } catch (out_of_range ex) {
                        client->send(Response::NotFound());
                    }
                }

                delete client;
            }
        }
    });
}

Handler HttpServer::find_route(Request &request) {
    auto path_pieces = split(request.path, '/');
    auto path_size = path_pieces.size();

    for (auto route : routes[request.method]) {
        auto route_pieces = split(route.first, '/');
        if (path_size == route_pieces.size()) {
            map<string, string> params;
            for (int i = 0; i < path_size; i++) {
                auto rp = route_pieces[i];
                auto pp = path_pieces[i];
                if (rp.find('{') == 0) {
                    auto key = trim(rp, "{}");
                    params[key] = pp;
                } else if (rp != pp) {
                    break;
                }
                if (i == path_size - 1) {
                    request.params = params;
                    return route.second;
                }
            }
        }
    }
    throw out_of_range("Route not found");
}
