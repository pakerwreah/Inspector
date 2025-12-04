//
// Created by Paker on 15/08/20.
//

#include "url.h"
#include "util.h"

namespace url {
    std::string decode(const std::string &_encoded) {
        std::string encoded = util::replaceAll(_encoded, "+", "%20");
        std::string ret;
        for (int i = 0; i < encoded.length(); i++) {
            if (static_cast<int>(encoded[i]) == 37) {
                ret += static_cast<char>(strtoul(encoded.substr(i + 1, 2).c_str(), nullptr, 16));
                i += 2;
            } else {
                ret += encoded[i];
            }
        }
        return ret;
    }

    std::map<std::string, std::string> params(const std::string &encoded) {
        std::map<std::string, std::string> params;
        auto pieces = util::split(encoded, '&');
        for (const std::string &piece : pieces) {
            auto p = util::split(piece, '=');
            params[p[0]] = p.size() == 2 ? decode(p[1]) : "";
        }
        return params;
    }
}
