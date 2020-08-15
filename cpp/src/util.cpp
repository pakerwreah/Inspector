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

    string join(const vector<string> &pieces, char glue) {
        string a;
        auto leng = pieces.size();
        for (int i = 0; i < leng; i++) {
            a += pieces[i];
            if (i < (leng - 1))
                a += glue;
        }
        return a;
    }

    vector<string> split(const string &str, char delim) {
        stringstream ss(str);
        string token;
        vector<string> parts;
        while (getline(ss, token, delim)) {
            parts.push_back(token);
        }
        return parts;
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
    vector<T> filter(const vector<T> &container, function<bool(const T&)> predicate) {
        vector<T> result;
        copy_if(container.begin(), container.end(), back_inserter(result), predicate);
        return result;
    }

    template vector<string> filter(const vector<string> &, function<bool(const string&)>);

    bool endsWith(const string &str, const string &suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }

    bool startsWith(const string &str, const string &prefix) {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }
}