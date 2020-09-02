//
// Created by Paker on 15/08/20.
//

#ifndef INSPECTOR_URL_H
#define INSPECTOR_URL_H

#include <map>
#include <string>

namespace url {
    std::string decode(const std::string &encoded);
    std::map<std::string, std::string> params(const std::string &encoded);
}

#endif //INSPECTOR_URL_H
