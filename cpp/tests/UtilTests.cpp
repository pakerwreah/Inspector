#include "catch.hpp"
#include "util.h"

#include <unistd.h>
#include <ctime>
#include <string>

using namespace std;
using namespace util;

TEST_CASE("util::uid") {
    string prev;
    for (int i = 0; i < 10; i++)
        REQUIRE(prev != uid());
}

TEST_CASE("util::timediff") {
    timeval start{5, 1500};
    timeval end{20, 2000};
    timeval expected{15, 500};
    timeval result = timediff(start, end);
    CHECK(timercmp(&result, &expected, ==));
}

TEST_CASE("util::benchmark") {
    timeval start = timestamp();
    sleep(1);
    const auto res = benchmark(start);
    long sec, usec;
    CHECK_NOTHROW(sec = res.at("sec"));
    CHECK_NOTHROW(usec = res.at("usec"));
    CHECK(sec >= 1);
    CHECK(usec < 10e6);
}

TEST_CASE("util::join") {
    SECTION("string pieces") {
        vector<string> parts = {"str1", "str2", "str3"};

        CHECK(join(parts, ',') == "str1,str2,str3");
        CHECK(join(parts, ", ") == "str1, str2, str3");
        CHECK(join(parts, string(", ")) == "str1, str2, str3");
    }

    SECTION("int pieces") {
        vector<int> parts = {1, 2, 3};

        CHECK(join(parts, ',') == "1,2,3");
        CHECK(join(parts, ", ") == "1, 2, 3");
        CHECK(join(parts, string(", ")) == "1, 2, 3");
    }
}

TEST_CASE("util::split") {
    SECTION("allow empty") {
        vector<string> expected = {"", "str1", "", "str3"};

        CHECK(split(",str1,,str3", ',') == expected);
        CHECK(split(", str1, , str3", ", ") == expected);
    }

    SECTION("does not allow empty") {
        vector<string> expected = {"str1", "str3"};

        CHECK(split(",str1,,str3", ',', false) == expected);
        CHECK(split(", str1, , str3", ", ", false) == expected);
    }

    SECTION("needle not found") {
        vector<string> expected = {"str1,str3"};

        CHECK(split("str1,str3", 'x') == expected);
        CHECK(split("str1,str3", "x") == expected);
    }

    SECTION("empty delim") {
        vector<string> expected = {"text"};

        CHECK(split("text", "") == expected);
    }
}

TEST_CASE("util::rtrim") {
    CHECK(rtrim("  text  ") == "  text");
    CHECK(rtrim("  text  ", "") == "  text  ");
    CHECK(rtrim("  text  ", " ") == "  text");
    CHECK(rtrim("  text  ", "t") == "  text  ");
    CHECK(rtrim("  text  ", " t") == "  tex");
}

TEST_CASE("util::ltrim") {
    CHECK(ltrim("  text  ") == "text  ");
    CHECK(ltrim("  text  ", "") == "  text  ");
    CHECK(ltrim("  text  ", " ") == "text  ");
    CHECK(ltrim("  text  ", "t") == "  text  ");
    CHECK(ltrim("  text  ", " t") == "ext  ");
}

TEST_CASE("util::trim") {
    CHECK(trim("  text  ") == "text");
    CHECK(trim("  text  ", "") == "  text  ");
    CHECK(trim("  text  ", " ") == "text");
    CHECK(trim("  text  ", "t") == "  text  ");
    CHECK(trim("  text  ", " t") == "ex");
}

TEST_CASE("util::endsWith") {
    CHECK_FALSE(endsWith("teste", "t"));
    CHECK_FALSE(endsWith("teste ", "e"));
    CHECK(endsWith("teste", "e"));
    CHECK(endsWith("teste", "te"));
}

TEST_CASE("util::startsWith") {
    CHECK_FALSE(startsWith("teste", "e"));
    CHECK_FALSE(startsWith(" teste", "t"));
    CHECK(startsWith("teste", "t"));
    CHECK(startsWith("teste", "te"));
}

TEST_CASE("util::replaceAll") {
    CHECK(replaceAll("teste", "", "x") == "teste");
    CHECK(replaceAll("teste", "e", "x") == "txstx");
    CHECK(replaceAll("teste", "e", "") == "tst");
    CHECK(replaceAll("teste", "te", "x") == "xsx");
    CHECK(replaceAll("teste", "x", "e") == "teste");
}

TEST_CASE("util::filter") {
    vector<int> values = {1, 2, 3, 4, 5};
    vector<int> expected = {1, 3, 5};

    CHECK(filter(values, [](int n) {
        return n % 2;
    }) == expected);
}

TEST_CASE("util::merge") {
    map<string, int> target = {
            {"a", 1},
            {"b", 1}
    };
    map<string, int> source = {
            {"a", 2},
            {"c", 2}
    };
    map<string, int> expected = {
            {"a", 2},
            {"b", 1},
            {"c", 2}
    };
    merge(target, source);
    CHECK(target == expected);
}
