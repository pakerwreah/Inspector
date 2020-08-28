//
// Created by Paker on 2020-08-23.
//

#ifndef INSPECTOR_RESPONSE_H
#define INSPECTOR_RESPONSE_H

#include <map>
#include <string>
#include "json.hpp"
#include "Http.h"

struct Response {
    Response(const nlohmann::json &data = nullptr, int code = 200, const std::string &content_type = Http::ContentType::JSON);

    operator std::string();

    int code;
    Headers headers;
    std::string body;

    static Response BadRequest() {
        return Response("Bad Request", 400);
    }

    static Response NotFound(const std::string &error) {
        return Response(error, 404);
    }

    static Response InternalError(const std::string &error) {
        return Response(error, 500);
    }
};

#endif //INSPECTOR_RESPONSE_H
