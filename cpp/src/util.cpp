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

    string join(const vector<string> &pieces, const char &glue) {
        string a;
        auto leng = pieces.size();
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

    // trim from end of string (right)
    string &rtrim(string &s, const char *t) {
        return s.erase(s.find_last_not_of(t) + 1);
    }

    // trim from beginning of string (left)
    string &ltrim(string &s, const char *t) {
        return s.erase(0, s.find_first_not_of(t));
    }

    // trim from both ends of string (right then left)
    string &trim(string &s, const char *t) {
        return ltrim(rtrim(s, t), t);
    }
}