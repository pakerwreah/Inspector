//
// Created by Paker on 2020-08-23.
//

#include "Response.h"
#include <sstream>

using namespace std;
using nlohmann::json;

Response::Response(const char *data, int code) : Response(data ? string(data) : "", code) {}

Response::Response(string data, int code) : code(code), body(std::move(data)) {
    headers[Http::ContentType::Key] = Http::ContentType::HTML;
}

Response::Response(const json &data, int code) : code(code), body(data.is_null() ? "" : data.dump()) {
    headers[Http::ContentType::Key] = Http::ContentType::JSON;
}

std::string Response::str() const {
    auto crlf = "\r\n";
    ostringstream resp;
    resp << "HTTP/1.1 " << code << " " << crlf
         << "Access-Control-Allow-Origin: *" << crlf;

    for (const auto &[name, value] : headers) {
        resp << name << ": " << value << crlf;
    }

    resp << "Content-Length: " << body.length() << crlf << crlf << body;

    return resp.str();
}
