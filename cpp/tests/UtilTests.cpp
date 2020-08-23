#include "catch.hpp"
#include "util.h"

#include <string>

using namespace std;
using namespace util;

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
