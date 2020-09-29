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
    int code;
    Headers headers;
    std::string body;

    Response(const char *data = "", int code = 200);
    Response(const std::string &data, int code = 200);
    Response(const nlohmann::json &data, int code = 200);

    operator std::string() const;

    static Response BadRequest(const std::string &error = "Bad Request") {
        return {{{"msg", error}}, 400};
    }

    static Response NotFound(const std::string &error = "Not Found") {
        return {{{"msg", error}}, 404};
    }

    static Response InternalError(const std::string &error = "Internal Error") {
        return {{{"msg", error}}, 500};
    }
};

#endif //INSPECTOR_RESPONSE_H
