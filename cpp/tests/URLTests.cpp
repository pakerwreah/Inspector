#include "catch.hpp"
#include "Http.h"
#include "url.h"

TEST_CASE("url::decode") {
    std::string value = url::decode("query%25%2F%26data%3D%28%26%3A%3F%29");
    std::string expected = "query%/&data=(&:?)";
    CHECK(value == expected);
}

TEST_CASE("url::params") {
    Params value = url::params("param1=query%25%2F&param2=data%3D%28%26%3A%3F%29");
    Params expected = {
            {"param1", "query%/"},
            {"param2", "data=(&:?)"}
    };
    CHECK(value == expected);
}
