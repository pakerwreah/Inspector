#include "catch.hpp"
#include "NetworkPlugin.h"
#include "MockClient.h"
#include "compress.hpp"

#include <unistd.h>

using namespace std;

static const int test_port = 50000;

TEST_CASE("NetworkPlugin") {
    Router router;
    Request request;
    shared_ptr requestClient = make_shared<MockClient>();
    shared_ptr responseClient = make_shared<MockClient>();

    request.headers = {
            {"Sec-WebSocket-Key", "secret"}
    };

    NetworkPlugin plugin(&router);
    CHECK_FALSE(plugin.isConnected());

    REQUIRE(request.parse("GET /network/request HTTP/1.1\r\n\r\n"));
    request.client = requestClient;
    REQUIRE_NOTHROW(router.handle(request));
    CHECK(plugin.isRequestConnected());

    CHECK_FALSE(plugin.isConnected());

    REQUIRE(request.parse("GET /network/response HTTP/1.1\r\n\r\n"));
    request.client = responseClient;
    REQUIRE_NOTHROW(router.handle(request));
    CHECK(plugin.isResponseConnected());

    CHECK(plugin.isConnected());

    SECTION("Send Request") {
        SECTION("Success") {
            REQUIRE_NOTHROW(plugin.sendRequest("1", "headers", "body"));
            CHECK(requestClient->sent == WebSocket::pack("1\nheaders\n" + gzip::compress("body", 4), true));
            CHECK(plugin.isRequestConnected());
            CHECK(plugin.isResponseConnected());
            CHECK(plugin.isConnected());
        }

        SECTION("Fail") {
            requestClient->connected = false;
            REQUIRE_NOTHROW(plugin.sendRequest("1", "headers", "body"));
            CHECK(requestClient->sent.empty());
            CHECK_FALSE(plugin.isRequestConnected());
            CHECK(plugin.isResponseConnected());
            CHECK_FALSE(plugin.isConnected());
        }
    }

    SECTION("Send Response") {
        SECTION("Success") {
            REQUIRE_NOTHROW(plugin.sendResponse("1", "headers", "body"));
            CHECK(responseClient->sent == WebSocket::pack("1\nheaders\n" + gzip::compress("body", 4), true));
            CHECK(plugin.isRequestConnected());
            CHECK(plugin.isResponseConnected());
            CHECK(plugin.isConnected());

            REQUIRE_NOTHROW(plugin.sendResponse("1", "headers", "body", true));
            CHECK(responseClient->sent == WebSocket::pack("1\nheaders\nbody", true));
            CHECK(plugin.isRequestConnected());
            CHECK(plugin.isResponseConnected());
            CHECK(plugin.isConnected());
        }

        SECTION("Fail") {
            responseClient->connected = false;
            REQUIRE_NOTHROW(plugin.sendResponse("1", "headers", "body"));
            CHECK(responseClient->sent.empty());
            CHECK(plugin.isRequestConnected());
            CHECK_FALSE(plugin.isResponseConnected());
            CHECK_FALSE(plugin.isConnected());
        }
    }
}
