//
// Created by Paker on 15/08/20.
//

#include "url.h"
#include "util.h"

using namespace std;

namespace url {
    string decode(const string &_encoded) {
        string encoded = util::replaceAll(_encoded, "+", "%20");
        string ret;
        char ch;
        int i, j;
        for (i = 0; i < encoded.length(); i++) {
            if (static_cast<int>(encoded[i]) == 37) {
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

    map<string, string> params(const string &encoded) {
        map<string, string> params;
        auto pieces = util::split(encoded, '&');
        for (const string &piece : pieces) {
            auto p = util::split(piece, '=');
            params[p[0]] = p.size() == 2 ? decode(p[1]) : "";
        }
        return params;
    }
}
