//
// Created by Paker on 2020-08-23.
//

#include "Response.h"
#include <sstream>

using namespace std;
using json = nlohmann::json;

Response::Response(const char *data, int code) : Response(data ? string(data) : "", code) {}

Response::Response(const string &data, int code) {
    headers[Http::ContentType::Key] = Http::ContentType::HTML;
    this->code = code;
    this->body = data;
}

Response::Response(const json &data, int code) {
    headers[Http::ContentType::Key] = Http::ContentType::JSON;
    this->code = code;
    this->body = data.is_null() ? "" : data.dump();
}

Response::operator std::string() const {
    const char *crlf = "\r\n";
    ostringstream resp;
    resp << "HTTP/1.1 " << code << " " << crlf
         << "Access-Control-Allow-Origin: *" << crlf;

    for (const auto &header : headers) {
        resp << header.first << ": " << header.second << crlf;
    }

    resp << "Content-Length: " << body.length() << crlf << crlf << body;

    return resp.str();
}
