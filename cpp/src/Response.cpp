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
        if (content_type == Http::ContentType::JSON) {
            resp = json{{"msg", data}}.dump();
        } else {
            resp = data.get<string>();
        }
    } else {
        resp = data.dump();
    }
    headers[Http::ContentType::Key] = content_type;
    this->body = resp;
    this->code = code;
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
