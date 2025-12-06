#include "catch.hpp"
#include "WebSocketPlugin.h"
#include "MockClient.h"

TEST_CASE("WebSocketPlugin - Connection") {
    Router router;
    Request request;
    Headers headers{{"Sec-WebSocket-Key", "secret"}};
    auto client = std::make_shared<MockClient>();

    WebSocketPlugin plugin(router);
    CHECK_FALSE(plugin.isConnected());

    SECTION("Success") {
        request = {.client = client, .headers = headers, .method = "GET", .path = "/plugins/ws/test"};
        Response response;
        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(plugin.isConnected());
        CHECK(response.str() == WebSocket::handshake(request).str());
    }

    SECTION("Fail") {
        request = {.client = client, .headers = headers, .method = "GET", .path = "/plugins/ws/path/test"};
        CHECK_THROWS_MATCHES(router.handle(request), std::out_of_range, Catch::Message("Route not found"));
        CHECK_FALSE(plugin.isConnected());
    }
}

TEST_CASE("WebSocketPlugin - Send message") {
    Router router;
    Request request;
    Response response;
    Headers headers;
    auto client = std::make_shared<MockClient>();

    WebSocketPlugin plugin(router);
    CHECK_FALSE(plugin.isConnected());

    headers = {{"Sec-WebSocket-Key", "secret"}};
    request = {.client = client, .headers = headers, .method = "GET", .path = "/plugins/ws/test"};
    REQUIRE_NOTHROW(response = router.handle(request));

    CHECK(plugin.isConnected());
    CHECK(response.str() == WebSocket::handshake(request).str());

    SECTION("Success") {
        REQUIRE_NOTHROW(plugin.sendMessage("test", "Test message"));
        CHECK(client->sent == WebSocket::pack("Test message", false));
    }

    SECTION("Wrong key") {
        REQUIRE_NOTHROW(plugin.sendMessage("wrong", "Test message"));
        CHECK(client->sent.empty());
        CHECK(plugin.isConnected());
    }

    SECTION("Fail") {
        client->connected = false;
        REQUIRE_NOTHROW(plugin.sendMessage("test", "Test message"));
        CHECK(client->sent.empty());
        CHECK_FALSE(plugin.isConnected());
    }
}
