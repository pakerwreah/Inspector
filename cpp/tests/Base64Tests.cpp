#include "catch.hpp"
#include "base64.h"

using namespace std;
using namespace base64;

TEST_CASE("base64::encode") {
    string value = encode("Lorem ipsum dolor sit amet");
    string expected = "TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQ=";
    CHECK(value == expected);
}

TEST_CASE("base64::decode") {
    string value = decode("TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQ=");
    string expected = "Lorem ipsum dolor sit amet";
    CHECK(value == expected);
}
