//
// Created by Paker on 2019-10-24.
//

#include "util.h"

namespace util {
    string join(const vector<string> &pieces, const char &glue) {
        string a;
        auto leng = pieces.size();
        for (int i = 0; i < leng; i++) {
            a += pieces[i];
            if (i < (leng - 1))
                a += glue;
        }
        return a;
    }

    vector<string> split(const string &str, const char &delim) {
        stringstream ss(str);
        string token;
        vector<string> parts;
        while (getline(ss, token, delim)) {
            parts.push_back(token);
        }
        return parts;
    }

    // trim from end of string (right)
    std::string &rtrim(std::string &s, const char *t) {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    std::string &ltrim(std::string &s, const char *t) {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    std::string &trim(std::string &s, const char *t) {
        return ltrim(rtrim(s, t), t);
    }
}