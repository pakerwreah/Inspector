#include "catch.hpp"
#include "compress.hpp"
#include "HttpServer.h"

using namespace std;

struct MockClient : public Client {
    string request;
    string response;

    string read() override {
        return request;
    }

    bool send(const string &data) override {
        response = data;
        return true;
    }
};

TEST_CASE_METHOD(HttpServer, "HttpServer - Invalid request") {
    auto client = make_shared<MockClient>();
    REQUIRE_NOTHROW(process(client));
    CHECK(client->response == string(Response::BadRequest()));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route not found") {
    auto client = make_shared<MockClient>();
    client->request = "GET /test/path HTTP/1.1\r\n\r\n";
    REQUIRE_NOTHROW(process(client));
    CHECK(client->response == string(Response::NotFound("Route not found")));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route found") {
    auto client = make_shared<MockClient>();
    Response expected("response data", 200, Http::ContentType::HTML);

    router.get("/test/path", [expected](const Request &, const Params &) {
        return expected;
    });

    client->request = "GET /test/path HTTP/1.1\r\n";

    SECTION("Response plain") {
        client->request += "\r\n";
        REQUIRE_NOTHROW(process(client));
        CHECK(client->response == string(expected));
    }

    SECTION("Response compressed") {
        expected.headers["Content-Encoding"] = "gzip";
        expected.body = gzip::compress(expected.body.c_str(), expected.body.size());
        client->request += "Accept-Encoding: gzip, deflate\r\n\r\n";
        REQUIRE_NOTHROW(process(client));
        CHECK(client->response == string(expected));
    }
}
