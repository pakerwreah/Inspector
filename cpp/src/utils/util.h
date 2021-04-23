//
// Created by Paker on 2019-10-24.
//

#ifndef INSPECTOR_UTIL_H
#define INSPECTOR_UTIL_H

#include <sys/time.h>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <functional>

namespace util {
    std::string uid();

    timeval timestamp();

    timeval timediff(const timeval &start, const timeval &end);

    std::map<std::string, long> benchmark(const timeval &start);

    std::vector<std::string> split(const std::string &str, char delim, bool allow_empty = true);
    std::vector<std::string> split(const std::string &str, const std::string &delim, bool allow_empty = true);

    const std::string whitespaces = " \t\n\r\f\v";

    // trim from end of string (right)
    std::string rtrim(const std::string &s, const std::string &t = whitespaces);

    // trim from beginning of string (left)
    std::string ltrim(const std::string &s, const std::string &t = whitespaces);

    // trim from both ends of string (right then left)
    std::string trim(const std::string &s, const std::string &t = whitespaces);

    bool endsWith(const std::string &str, const std::string &suffix);

    bool startsWith(const std::string &str, const std::string &prefix);

    std::string replaceAll(const std::string &str, const std::string &needle, const std::string &replacement);
}

namespace util {
    template<typename Container>
    using value_type = typename Container::value_type;

    template<typename T>
    std::vector<value_type<T>> filter(const T &container, std::function<bool(const value_type<T> &)> predicate) {
        std::vector<value_type<T>> result;
        copy_if(container.begin(), container.end(), back_inserter(result), predicate);
        return result;
    }

    template<typename Iterable, typename T>
    std::string join(const Iterable &pieces, const T &glue) {
        std::ostringstream os;
        for (const auto &piece : pieces) {
            os << piece;
            if (&piece != &pieces.back())
                os << glue;
        }
        return os.str();
    }

    template<typename K, typename V>
    void merge(std::map<K, V> &target, const std::map<K, V> &source) {
        for (const auto &[key, val] : source) {
            target[key] = val;
        }
    }
}

#endif //INSPECTOR_UTIL_H
