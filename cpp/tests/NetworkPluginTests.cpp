#include "catch.hpp"
#include "NetworkPlugin.h"
#include "MockClient.h"
#include "compress.hpp"

TEST_CASE("NetworkPlugin") {
    Router router;
    Request request;
    Response response;
    Headers headers{{"Sec-WebSocket-Key", "secret"}};
    auto requestClient = std::make_shared<MockClient>();
    auto responseClient = std::make_shared<MockClient>();

    NetworkPlugin plugin(router);
    CHECK_FALSE(plugin.isConnected());

    request = {.client = requestClient, .headers = headers, .method = "GET", .path = "/network/request"};
    REQUIRE_NOTHROW(response = router.handle(request));
    CHECK(response.str() == WebSocket::handshake(request).str());
    CHECK(plugin.isRequestConnected());

    CHECK_FALSE(plugin.isConnected());

    request = {.client = responseClient, .headers = headers, .method = "GET", .path = "/network/response"};
    REQUIRE_NOTHROW(response = router.handle(request));
    CHECK(response.str() == WebSocket::handshake(request).str());
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
