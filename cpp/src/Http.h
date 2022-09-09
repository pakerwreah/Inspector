//
// Created by Paker on 25/08/20.
//

#pragma once

#include <map>

namespace Http::ContentType {
    const auto Key = "Content-Type";
    const auto HTML = "text/html";
    const auto JSON = "application/json";
    const auto URL_ENCODED = "application/x-www-form-urlencoded";
}

typedef std::map<std::string, std::string> Headers;
typedef std::map<std::string, std::string> Params;
