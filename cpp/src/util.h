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

    static const char *whitespace = " \t\n\r\f\v";

    // trim from end of string (right)
    std::string &rtrim(std::string &s, const char *t = whitespace);

    // trim from beginning of string (left)
    std::string &ltrim(std::string &s, const char *t = whitespace);

    // trim from both ends of string (right then left)
    std::string &trim(std::string &s, const char *t = whitespace);
}

#endif //INSPECTOR_UTIL_H
