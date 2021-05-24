//
// Created by Paker on 2020-08-23.
//

#include "Request.h"
#include "picohttpparser.h"
#include <sstream>

using namespace std;

Request::Request(shared_ptr<Client> client)
        : client(client) {}

Request::Request(const string &method, const string &path, const string &body)
        : method(method), path(path), body(body) {}

Request::Request(const string &method, const string &path, std::shared_ptr<Client> client, const Headers &headers)
        : method(method), path(path), client(client), headers(headers) {}

bool Request::parse(const string &plain) {
    const char *method, *path;
    size_t path_len, method_len;
    int minor_version;
    size_t num_headers = 50;
    struct phr_header headers[50];
    const char *buffer = plain.c_str();

    int body_start = phr_parse_request(
            buffer, plain.length(),
            &method, &method_len,
            &path, &path_len,
            &minor_version,
            headers, &num_headers,
            0
    );

    if (body_start > 0) {
        for (int i = 0; i < num_headers; i++) {
            auto header = headers[i];
            auto name = string(header.name, header.name_len);
            auto value = string_view(header.value, header.value_len);
            this->headers[name] = value;
        }

        this->method = string_view(method, method_len);
        this->path = string_view(path, path_len);
        this->body = plain.substr(body_start);

        return true;
    }

    return false;
}

Request::operator std::string() const {
    const char *crlf = "\r\n";
    ostringstream resp;
    resp << method << " " << path << " HTTP/1.1" << crlf;

    for (const auto &header : headers) {
        resp << header.first << ": " << header.second << crlf;
    }

    resp << "Content-Length: " << body.length() << crlf << crlf << body;

    return resp.str();
}
