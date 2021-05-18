//
// Created by Paker on 15/08/20.
//

#pragma once

#include <map>
#include <string>

namespace url {
    std::string decode(const std::string &encoded);
    std::map<std::string, std::string> params(const std::string &encoded);
}
