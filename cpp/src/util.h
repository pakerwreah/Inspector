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

namespace util {

    timeval timestamp();

    timeval timediff(const timeval &start, const timeval &end);

    json benchmark(const timeval &start);

    template<typename T>
    std::string join(const std::vector<std::string> &pieces, const T &glue);

    std::vector<std::string> split(const std::string &str, char delim, bool allow_empty = true);
    std::vector<std::string> split(const std::string &str, const std::string &delim, bool allow_empty = true);

    static const char *whitespace = " \t\n\r\f\v";

    // trim from end of string (right)
    std::string rtrim(const std::string &s, const std::string &t = whitespace);

    // trim from beginning of string (left)
    std::string ltrim(const std::string &s, const std::string &t = whitespace);

    // trim from both ends of string (right then left)
    std::string trim(const std::string &s, const std::string &t = whitespace);

    template<typename T>
    std::vector<T> filter(const std::vector<T> &container, std::function<bool(const T &)> predicate);

    bool endsWith(const std::string &str, const std::string &suffix);

    bool startsWith(const std::string &str, const std::string &prefix);

    std::string replaceAll(const std::string &str, const std::string &needle, const std::string &replacement);
}

#endif //INSPECTOR_UTIL_H
