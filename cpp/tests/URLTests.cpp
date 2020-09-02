#include "catch.hpp"
#include "url.h"

using namespace std;
using namespace url;

TEST_CASE("url::decode") {
    string value = decode("query%25%2F%26data%3D%28%26%3A%3F%29");
    string expected = "query%/&data=(&:?)";
    CHECK(value == expected);
}

TEST_CASE("url::params") {
    map<string, string> value = params("param1=query%25%2F&param2=data%3D%28%26%3A%3F%29");
    map<string, string> expected = {
            {"param1", "query%/"},
            {"param2", "data=(&:?)"}
    };
    CHECK(value == expected);
}
