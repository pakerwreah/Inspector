//
// Created by Paker on 2020-08-23.
//

#ifndef INSPECTOR_RESPONSE_H
#define INSPECTOR_RESPONSE_H

#include <map>
#include <string>
#include "libs/json.hpp"

namespace ContentType {
    const std::string HTML = "text/html";
    const std::string JSON = "application/json";
    const std::string URL_ENCODED = "application/x-www-form-urlencoded";
}

struct Response {
    Response(const nlohmann::json &data = nullptr, int code = 200, const std::string &content_type = ContentType::JSON);

    operator std::string();

    int code;
    std::map<std::string, std::string> headers;
    std::string content_type;
    std::string body;

    static Response NotFound() {
        return Response("Route not found", 404);
    }

    static Response InternalError() {
        return Response("Internal Error", 500);
    }
};

#endif //INSPECTOR_RESPONSE_H
