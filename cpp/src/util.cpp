//
// Created by Paker on 2019-10-24.
//

#include "util.h"

using namespace std;
using namespace nlohmann;

namespace util {
    timeval timestamp() {
        struct timeval now;
        gettimeofday(&now, nullptr);
        return now;
    }

    timeval timediff(const timeval &start, const timeval &end) {
        struct timeval duration;
        timersub(&end, &start, &duration);
        return duration;
    }

    json benchmark(const timeval &start) {
        auto end = timestamp();
        auto diff = timediff(start, end);
        return {{"sec",  diff.tv_sec},
                {"usec", diff.tv_usec}};
    }

    template<typename T>
    string join(const vector<string> &pieces, const T &glue) {
        ostringstream os;
        for (const string &piece : pieces) {
            os << piece;
            if (&piece != &pieces.back())
                os << glue;
        }
        return os.str();
    }

    template string join(const vector<string> &pieces, const char &glue);

    template string join(const vector<string> &pieces, const string &glue);

    vector<string> split(const string &str, char delim, bool allow_empty) {
        return split(str, string(1, delim), allow_empty);
    }

    vector<string> split(const string &str, const string &delim, bool allow_empty) {
        assert(!delim.empty());
        vector<string> tokens;
        size_t prev = 0, pos = 0;
        do {
            pos = str.find(delim, prev);
            if (pos == string::npos) {
                pos = str.length();
            }
            string token = str.substr(prev, pos - prev);
            if (allow_empty || !token.empty()) {
                tokens.push_back(token);
            }
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());
        return tokens;
    }

    // trim from end of string (right)
    string rtrim(const string &s, const string &t) {
        return string(s).erase(s.find_last_not_of(t) + 1);
    }

    // trim from beginning of string (left)
    string ltrim(const string &s, const string &t) {
        return string(s).erase(0, s.find_first_not_of(t));
    }

    // trim from both ends of string (right then left)
    string trim(const string &s, const string &t) {
        return ltrim(rtrim(s, t), t);
    }

    template<typename T>
    vector<T> filter(const vector<T> &container, function<bool(const T &)> predicate) {
        vector<T> result;
        copy_if(container.begin(), container.end(), back_inserter(result), predicate);
        return result;
    }

    template vector<string> filter(const vector<string> &, function<bool(const string &)>);

    bool endsWith(const string &str, const string &suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }

    bool startsWith(const string &str, const string &prefix) {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }

    std::string replaceAll(const string &str, const string &needle, const string &replacement) {
        assert(!needle.empty());
        string copy = str;
        auto pos = copy.find(needle);
        while (pos != string::npos) {
            copy.replace(pos, needle.size(), replacement);
            pos = copy.find(needle, pos + replacement.size());
        }
        return copy;
    }
}