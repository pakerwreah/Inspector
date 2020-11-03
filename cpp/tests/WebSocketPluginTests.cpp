#include "catch.hpp"
#include "WebSocketPlugin.h"
#include "MockClient.h"

using namespace std;

TEST_CASE("WebSocketPlugin - Connection") {
    Router router;
    Request request;
    Headers headers{{"Sec-WebSocket-Key", "secret"}};
    shared_ptr client = make_shared<MockClient>();

    WebSocketPlugin plugin(router);
    CHECK_FALSE(plugin.isConnected());

    SECTION("Success") {
        request = {"GET", "/plugins/ws/test", client, headers};
        Response response;
        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(plugin.isConnected());
        CHECK(string(response) == string(WebSocket::handshake(request)));
    }

    SECTION("Fail") {
        request = {"GET", "/plugins/ws/path/test", client, headers};
        CHECK_THROWS_MATCHES(router.handle(request), out_of_range, Catch::Message("Route not found"));
        CHECK_FALSE(plugin.isConnected());
    }
}

TEST_CASE("WebSocketPlugin - Send message") {
    Router router;
    Request request;
    Response response;
    Headers headers;
    shared_ptr client = make_shared<MockClient>();

    WebSocketPlugin plugin(router);
    CHECK_FALSE(plugin.isConnected());

    headers = {{"Sec-WebSocket-Key", "secret"}};
    request = {"GET", "/plugins/ws/test", client, headers};
    REQUIRE_NOTHROW(response = router.handle(request));

    CHECK(plugin.isConnected());
    CHECK(string(response) == string(WebSocket::handshake(request)));

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
