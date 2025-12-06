//
// Created by Paker on 2020-08-23.
//

#pragma once

#include "json.hpp"
#include "Http.h"

struct Response {
    int code;
    Headers headers;
    std::string body;

    explicit Response(const char *data = "", int code = 200);
    explicit Response(std::string data, int code = 200);
    explicit Response(const nlohmann::json &data, int code = 200);

    std::string str() const;
    operator std::string() const { return str(); }

    static Response BadRequest(const std::string &error = "Bad Request") {
        return Response({{"msg", error}}, 400);
    }

    static Response NotFound(const std::string &error = "Not Found") {
        return Response({{"msg", error}}, 404);
    }

    static Response InternalError(const std::string &error = "Internal Error") {
        return Response({{"msg", error}}, 500);
    }
};
