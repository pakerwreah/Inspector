#include "catch.hpp"
#include "Request.h"

using namespace std;

TEST_CASE("Request - invalid") {
    Request request;
    REQUIRE_FALSE(request.parse("GET HTTP/1.1\r\n\r\n"));
    CHECK(request.method.empty());
    CHECK(request.path.empty());
    CHECK(request.headers.empty());
    CHECK(request.body.empty());
}

TEST_CASE("Request - incomplete") {
    Request request;
    REQUIRE_FALSE(request.parse("GET /test/path HTTP/1.1\r\n"));
    CHECK(request.method.empty());
    CHECK(request.path.empty());
    CHECK(request.headers.empty());
    CHECK(request.body.empty());
}

TEST_CASE("Request - parsed") {
    Request request;
    string plain_request =
            "/test/path?a=1&b=2 HTTP/1.1\r\n"
            "Host: www.google.com\r\n"
            "User-Agent: curl/7.64.1\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Accept: */*\r\n\r\n";

    Headers expected_headers = {
            {"Content-Type", "application/x-www-form-urlencoded"},
            {"Host",         "www.google.com"},
            {"User-Agent",   "curl/7.64.1"},
            {"Accept",       "*/*"}
    };

    SECTION("GET") {
        plain_request = "GET " + plain_request;
        REQUIRE(request.parse(plain_request));
        CHECK(request.method == "GET");
        CHECK(request.body.empty());
    }

    SECTION("POST") {
        plain_request = "POST " + plain_request + "this is a body";
        REQUIRE(request.parse(plain_request));
        CHECK(request.method == "POST");
        CHECK(request.body == "this is a body");
    }

    CHECK(request.path == "/test/path?a=1&b=2");
    CHECK(request.headers == expected_headers);
}
