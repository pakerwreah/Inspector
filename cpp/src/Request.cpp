//
// Created by Paker on 2020-08-23.
//

#include "Request.h"
#include "libs/picohttpparser.h"

using namespace std;

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

bool Request::is_valid() {
    return !method.empty();
}