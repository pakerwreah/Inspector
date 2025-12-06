#include "catch.hpp"
#include "base64.h"

TEST_CASE("base64::encode") {
    std::string value = base64::encode("Lorem ipsum dolor sit amet");
    std::string expected = "TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQ=";
    CHECK(value == expected);
}

TEST_CASE("base64::decode") {
    std::string value = base64::decode("TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQ=");
    std::string expected = "Lorem ipsum dolor sit amet";
    CHECK(value == expected);
}
