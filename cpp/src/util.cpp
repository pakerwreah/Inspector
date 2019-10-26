//
// Created by Paker on 2019-10-24.
//

#include "util.h"

namespace util {
    string join(const vector<string> &pieces, const char &glue) {
        string a;
        int leng = pieces.size();
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
}