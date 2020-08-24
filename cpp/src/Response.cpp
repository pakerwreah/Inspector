//
// Created by Paker on 2020-08-23.
//

#include "Response.h"
#include <sstream>

using namespace std;
using json = nlohmann::json;

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
    const char *crlf = "\r\n";
    ostringstream resp;
    resp << "HTTP/1.1 " << code << " " << crlf
         << "Content-Type: " << content_type << crlf
         << "Content-Length: " << body.length() << crlf
         << "Access-Control-Allow-Origin: *" << crlf;

    for (const auto &header : headers) {
        resp << header.first << ": " << header.second << crlf;
    }

    resp << crlf << body;
    return resp.str();
}