//
// Created by Paker on 2019-10-24.
//

#ifndef INSPECTOR_UTIL_H
#define INSPECTOR_UTIL_H

#include <string>
#include <sstream>
#include <vector>

namespace util {
    using namespace std;

    string join(const vector<string> &pieces, const char &glue);

    vector<string> split(const string &str, const char &delim);
}

#endif //INSPECTOR_UTIL_H
