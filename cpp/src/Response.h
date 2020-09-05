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
    Response(const char *data = "", int code = 200, const std::string &content_type = Http::ContentType::JSON);
    Response(const std::string &data, int code = 200, const std::string &content_type = Http::ContentType::JSON);
    Response(const nlohmann::json &data, int code = 200);

    operator std::string() const;

    int code;
    Headers headers;
    std::string body;

    static Response BadRequest() {
        return {"Bad Request", 400};
    }

    static Response NotFound(const std::string &error) {
        return {error, 404};
    }

    static Response InternalError(const std::string &error) {
        return {error, 500};
    }
};

#endif //INSPECTOR_RESPONSE_H
