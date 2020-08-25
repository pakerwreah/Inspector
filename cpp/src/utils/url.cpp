//
// Created by Paker on 15/08/20.
//

#include "url.h"
#include "util.h"

namespace url {
    std::string decode(const std::string &_encoded) {
        std::string encoded = util::replaceAll(_encoded, "+", "%20");
        std::string ret;
        char ch;
        int i, j;
        for (i = 0; i < encoded.length(); i++) {
            if (int(encoded[i]) == 37) {
                sscanf(encoded.substr(i + 1, 2).c_str(), "%x", &j);
                ch = static_cast<char>(j);
                ret += ch;
                i = i + 2;
            } else {
                ret += encoded[i];
            }
        }
        return ret;
    }
}