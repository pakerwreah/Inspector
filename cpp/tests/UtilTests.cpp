#include "catch.hpp"
#include "util.h"

#include <unistd.h>
#include <ctime>
#include <string>

TEST_CASE("util::uid") {
    std::string prev;
    for (int i = 0; i < 10; i++)
        REQUIRE(prev != util::uid());
}

TEST_CASE("util::timediff") {
    timeval start{5, 1500};
    timeval end{20, 2000};
    timeval expected{15, 500};
    timeval result = util::timediff(start, end);
    CHECK(timercmp(&result, &expected, ==));
}

TEST_CASE("util::benchmark") {
    timeval start = util::timestamp();
    sleep(1);
    const auto res = util::benchmark(start);
    long sec, usec;
    CHECK_NOTHROW(sec = res.at("sec"));
    CHECK_NOTHROW(usec = res.at("usec"));
    CHECK(sec >= 1);
    CHECK(usec < 10e6);
}

TEST_CASE("util::join") {
    SECTION("string pieces") {
        std::vector<std::string> parts = {"str1", "str2", "str3"};

        CHECK(util::join(parts, ',') == "str1,str2,str3");
        CHECK(util::join(parts, ", ") == "str1, str2, str3");
        CHECK(util::join(parts, std::string(", ")) == "str1, str2, str3");
    }

    SECTION("int pieces") {
        std::vector<int> parts = {1, 2, 3};

        CHECK(util::join(parts, ',') == "1,2,3");
        CHECK(util::join(parts, ", ") == "1, 2, 3");
        CHECK(util::join(parts, std::string(", ")) == "1, 2, 3");
    }
}

TEST_CASE("util::split") {
    SECTION("allow empty") {
        std::vector<std::string> expected = {"", "str1", "", "str3"};

        CHECK(util::split(",str1,,str3", ',') == expected);
        CHECK(util::split(", str1, , str3", ", ") == expected);
    }

    SECTION("does not allow empty") {
        std::vector<std::string> expected = {"str1", "str3"};

        CHECK(util::split(",str1,,str3", ',', false) == expected);
        CHECK(util::split(", str1, , str3", ", ", false) == expected);
    }

    SECTION("needle not found") {
        std::vector<std::string> expected = {"str1,str3"};

        CHECK(util::split("str1,str3", 'x') == expected);
        CHECK(util::split("str1,str3", "x") == expected);
    }

    SECTION("empty delim") {
        std::vector<std::string> expected = {"text"};

        CHECK(util::split("text", "") == expected);
    }
}

TEST_CASE("util::rtrim") {
    CHECK(util::rtrim("  text  ") == "  text");
    CHECK(util::rtrim("  text  ", "") == "  text  ");
    CHECK(util::rtrim("  text  ", " ") == "  text");
    CHECK(util::rtrim("  text  ", "t") == "  text  ");
    CHECK(util::rtrim("  text  ", " t") == "  tex");
}

TEST_CASE("util::ltrim") {
    CHECK(util::ltrim("  text  ") == "text  ");
    CHECK(util::ltrim("  text  ", "") == "  text  ");
    CHECK(util::ltrim("  text  ", " ") == "text  ");
    CHECK(util::ltrim("  text  ", "t") == "  text  ");
    CHECK(util::ltrim("  text  ", " t") == "ext  ");
}

TEST_CASE("util::trim") {
    CHECK(util::trim("  text  ") == "text");
    CHECK(util::trim("  text  ", "") == "  text  ");
    CHECK(util::trim("  text  ", " ") == "text");
    CHECK(util::trim("  text  ", "t") == "  text  ");
    CHECK(util::trim("  text  ", " t") == "ex");
}

TEST_CASE("util::endsWith") {
    CHECK_FALSE(util::endsWith("teste", "t"));
    CHECK_FALSE(util::endsWith("teste ", "e"));
    CHECK(util::endsWith("teste", "e"));
    CHECK(util::endsWith("teste", "te"));
}

TEST_CASE("util::startsWith") {
    CHECK_FALSE(util::startsWith("teste", "e"));
    CHECK_FALSE(util::startsWith(" teste", "t"));
    CHECK(util::startsWith("teste", "t"));
    CHECK(util::startsWith("teste", "te"));
}

TEST_CASE("util::replaceAll") {
    CHECK(util::replaceAll("teste", "", "x") == "teste");
    CHECK(util::replaceAll("teste", "e", "x") == "txstx");
    CHECK(util::replaceAll("teste", "e", "") == "tst");
    CHECK(util::replaceAll("teste", "te", "x") == "xsx");
    CHECK(util::replaceAll("teste", "x", "e") == "teste");
}

TEST_CASE("util::filter") {
    std::vector values = {1, 2, 3, 4, 5};
    std::vector expected = {1, 3, 5};

    CHECK(util::filter(values, [](int n) {
        return n % 2;
    }) == expected);
}

TEST_CASE("util::merge") {
    std::map<std::string, int> target = {
            {"a", 1},
            {"b", 1}
    };
    std::map<std::string, int> source = {
            {"a", 2},
            {"c", 2}
    };
    std::map<std::string, int> expected = {
            {"a", 2},
            {"b", 1},
            {"c", 2}
    };
    util::merge(target, source);
    CHECK(target == expected);
}
