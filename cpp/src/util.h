//
// Created by Paker on 2019-10-24.
//

#ifndef INSPECTOR_UTIL_H
#define INSPECTOR_UTIL_H

#include <string>
#include <sstream>
#include <vector>
#include <sys/time.h>
#include "libs/json.hpp"

using json = nlohmann::json;

using namespace std;

namespace util {

    timeval timestamp();

    timeval timediff(const timeval &start, const timeval &end);

    json benchmark(const timeval &start);

    string join(const vector<string> &pieces, const char &glue);

    vector<string> split(const string &str, const char &delim);

    static const char *whitespace = " \t\n\r\f\v";

    // trim from end of string (right)
    string &rtrim(string &s, const char *t = whitespace);

    // trim from beginning of string (left)
    string &ltrim(string &s, const char *t = whitespace);

    // trim from both ends of string (right then left)
    string &trim(string &s, const char *t = whitespace);
}

#endif //INSPECTOR_UTIL_H
