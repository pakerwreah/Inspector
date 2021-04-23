//
// Created by Paker on 25/08/20.
//

#ifndef INSPECTOR_HTTP_H
#define INSPECTOR_HTTP_H

#include <string>
#include <map>

namespace Http {
    namespace ContentType {
        const std::string Key = "Content-Type";
        const std::string HTML = "text/html";
        const std::string JSON = "application/json";
        const std::string URL_ENCODED = "application/x-www-form-urlencoded";
    }
}

typedef std::map<std::string, std::string> Headers;
typedef std::map<std::string, std::string> Params;

#endif //INSPECTOR_HTTP_H
