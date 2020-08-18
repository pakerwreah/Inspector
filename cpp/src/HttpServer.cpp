//
// Created by Paker on 2019-10-23.
//

#include <sstream>
#include <thread>
#include "HttpServer.h"
#include "Socket.h"
#include "libs/picohttpparser.h"
#include "libs/compress.hpp"
#include "libs/url.h"
#include "util.h"

using namespace util;

Request::Request(const string &plain, shared_ptr<Socket> client) {
    socket = client;

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

Response::Response(const json &data, int code, const string &content_type) {
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
    constexpr auto crlf = "\r\n";
    ostringstream resp;
    resp << "HTTP/1.1 " << code << " " << crlf
         << "Content-Type: " << content_type << crlf
         << "Content-Length: " << body.length() << crlf
         << "Access-Control-Allow-Origin: *" << crlf;

    for (auto header : headers) {
        resp << header.first << ": " << header.second << crlf;
    }

    resp << crlf << body;
    return resp.str();
}

void HttpServer::request(const string &method, const string &path, Handler handler) {
    routes[method][path] = handler;
}

void HttpServer::get(const string &path, Handler handler) {
    request("GET", path, handler);
}

void HttpServer::post(const string &path, Handler handler) {
    request("POST", path, handler);
}

void HttpServer::put(const string &path, Handler handler) {
    request("PUT", path, handler);
}

void HttpServer::request(const string &path, Handler handler) {
    request("*", path, handler);
}

void HttpServer::stop() {
    _stop = true;
}

thread *HttpServer::start(int port) {
    _stop = false;
    return new thread([this, port] {
        Socket server;
        server.create();

        while (!_stop) {
            if (server.bind(port) && server.listen()) {
                for (int i = 0; !_stop && i < 3; i++) {
                    Socket *_client;
                    if (server.accept(_client)) {
                        i = -1;
                        auto client = shared_ptr<Socket>(_client);
                        thread(&HttpServer::process, this, client).detach();
                    }
                }
            }
            sleep(1); // just to avoid an infinite cpu hogging loop
        }
    });
}

void HttpServer::process(shared_ptr<Socket> client) const {
    string plain, buf;
    bool valid = false;

    for (int i = 0; i < 3 && !valid; i++) {
        while (client->recv(buf, timeval{0, 10000})) {
            plain += buf;
        }

        Request request(plain, client);

        if ((valid = request.valid())) {
            try {
                if (request.method == "OPTIONS") {
                    Response response;
                    response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT";

                    client->send(response);

                } else {
                    Params params;
                    Handler handler = find_handler(request, &params);
                    auto response = handler(request, params);

                    if (response.body.size() && request.headers["Accept-Encoding"].find("gzip") >= 0) {
                        response.body = gzip::compress(response.body.data(), response.body.size());
                        response.headers["Content-Encoding"] = "gzip";
                    }

                    client->send(response);
                }
            } catch (out_of_range ex) {
                client->send(Response::NotFound());
            }
        }
    }

    if (!valid) {
        client->send(Response::InternalError());
    }
}

static void parse_urlencoded(const string &urlencoded, Params *params) {
    auto pieces = split(urlencoded, '&');
    for (const string &piece : pieces) {
        auto p = split(piece, '=');
        params->operator[](p[0]) = p.size() == 2 ? url_decode(p[1]) : "";
    }
}

Handler HttpServer::find_handler(const Request &request, Params *params) const {
    auto pieces = split(request.path, '?');
    auto path = pieces[0];

    // extract query params
    if (request.method == "GET" && pieces.size() > 1) {
        parse_urlencoded(pieces[1], params);
    } else {
        // extract body params
        auto type = request.headers.find("Content-Type");
        if (type != request.headers.end() && type->second.find(ContentType::URL_ENCODED) != string::npos) {
            parse_urlencoded(request.body, params);
        }
    }

    // normalize plugin name
    auto plugin_pieces = util::split(path, "plugins/api/");

    if (plugin_pieces.size() > 1) {
        auto plugin = util::replaceAll(plugin_pieces[1], "/", "-");
        path = plugin_pieces[0] + "plugins/api/" + plugin;
    }

    // find handler
    auto path_pieces = split(path, '/');
    auto path_size = path_pieces.size();

    // check for method then wildcard
    for (const char *method : {request.method.c_str(), "*"}) {
        auto m_routes = routes.find(method);
        if (m_routes != routes.end())
            for (const auto &route : m_routes->second) {
                auto route_pieces = split(route.first, '/');
                if (path_size != route_pieces.size()) continue;
                for (int i = 0; i < path_size; i++) {
                    auto rp = route_pieces[i];
                    auto pp = path_pieces[i];
                    if (rp.find('{') == 0) {
                        auto key = trim(rp, "{}");
                        params->operator[](key) = pp;
                    } else if (rp != pp) {
                        break;
                    }
                    if (i == path_size - 1) {
                        return route.second;
                    }
                }
            }
    }

    throw out_of_range("Route not found");
}
