//
// Created by Paker on 2019-10-24.
//

#include "util.h"


namespace util {
    std::string uid() {
        timeval tp{};
        gettimeofday(&tp, nullptr);
        return std::to_string(tp.tv_sec) + "-" + std::to_string(tp.tv_usec);
    }

    timeval timestamp() {
        timeval now{};
        gettimeofday(&now, nullptr);
        return now;
    }

    timeval timediff(const timeval &start, const timeval &end) {
        timeval duration{};
        timersub(&end, &start, &duration);
        return duration;
    }

    std::map<std::string, long> benchmark(const timeval &start) {
        auto end = timestamp();
        auto diff = timediff(start, end);
        return {
            {"sec", diff.tv_sec},
            {"usec", diff.tv_usec},
        };
    }

    std::vector<std::string> split(const std::string &str, char delim, bool allow_empty) {
        return split(str, std::string(1, delim), allow_empty);
    }

    std::vector<std::string> split(const std::string &str, const std::string &delim, bool allow_empty) {
        if (delim.empty()) return {str};

        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) {
                pos = str.length();
            }
            std::string token = str.substr(prev, pos - prev);
            if (allow_empty || !token.empty()) {
                tokens.push_back(token);
            }
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());

        return tokens;
    }

    // trim from end of string (right)
    std::string rtrim(const std::string &s, const std::string &t) {
        return std::string(s).erase(s.find_last_not_of(t) + 1);
    }

    // trim from beginning of string (left)
    std::string ltrim(const std::string &s, const std::string &t) {
        return std::string(s).erase(0, s.find_first_not_of(t));
    }

    // trim from both ends of string (right then left)
    std::string trim(const std::string &s, const std::string &t) {
        return ltrim(rtrim(s, t), t);
    }

    bool endsWith(const std::string &str, const std::string &suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }

    bool startsWith(const std::string &str, const std::string &prefix) {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }

    std::string replaceAll(const std::string &str, const std::string &needle, const std::string &replacement) {
        if (needle.empty()) return str;

        std::string copy = str;
        auto pos = copy.find(needle);
        while (pos != std::string::npos) {
            copy.replace(pos, needle.size(), replacement);
            pos = copy.find(needle, pos + replacement.size());
        }
        return copy;
    }
}
