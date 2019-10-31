//
// Created by Paker on 2019-10-24.
//

#ifndef INSPECTOR_UTIL_H
#define INSPECTOR_UTIL_H

#include <string>
#include <sstream>
#include <vector>
#include <sys/time.h>

namespace util {
    using namespace std;

    typedef unsigned long long timestamp_t;

    timestamp_t timestamp();

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
