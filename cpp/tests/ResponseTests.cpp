#include "catch.hpp"
#include "Response.h"

using namespace std;
using json = nlohmann::json;

TEST_CASE("Response - NULL") {
    Response response(nullptr);
    CHECK(response.code == 200);
    CHECK(response.headers == Headers{{"Content-Type", "application/json"}});
    CHECK(response.body == R"({"msg":""})");
}

TEST_CASE("Response - Fail") {
    Headers expected_headers = {
            {"Content-Type", "application/json"}
    };
    string expected_response =
            "HTTP/1.1 500 \r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 33\r\n"
            "\r\n"
            R"({"msg":"This request has failed"})";

    Response response("This request has failed", 500);
    CHECK(response.code == 500);
    CHECK(response.body == R"({"msg":"This request has failed"})");
    CHECK(response.headers == expected_headers);
    CHECK(string(response) == expected_response);
}

TEST_CASE("Response - JSON") {
    Headers expected_headers = {
            {"Content-Type", "application/json"}
    };
    string expected_response =
            "HTTP/1.1 200 \r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 11\r\n"
            "\r\n"
            R"({"ok":true})";

    Response response(json{{"ok", true}});
    CHECK(response.code == 200);
    CHECK(response.body == R"({"ok":true})");
    CHECK(response.headers == expected_headers);
    CHECK(string(response) == expected_response);
}

TEST_CASE("Response - HTML") {
    Headers expected_headers = {
            {"Content-Type", "text/html"}
    };
    string expected_response =
            "HTTP/1.1 200 \r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 14\r\n"
            "\r\n"
            "this is a body";

    Response response("this is a body", 200, Http::ContentType::HTML);
    CHECK(response.code == 200);
    CHECK(response.body == "this is a body");
    CHECK(response.headers == expected_headers);
    CHECK(string(response) == expected_response);
}
