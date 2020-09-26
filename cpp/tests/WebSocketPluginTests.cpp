#include "catch.hpp"
#include "WebSocketPlugin.h"
#include "MockClient.h"
#include "compress.hpp"

using namespace std;

TEST_CASE("WebSocketPlugin - Connection") {
    Router router;
    Request request;
    shared_ptr client = make_shared<MockClient>();

    request.headers = {
            {"Sec-WebSocket-Key", "secret"}
    };

    WebSocketPlugin plugin(&router);
    CHECK_FALSE(plugin.isConnected());

    SECTION("Success") {
        REQUIRE(request.parse("GET /plugins/ws/test HTTP/1.1\r\n\r\n"));
        request.client = client;
        Response response;
        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(plugin.isConnected());
        CHECK(string(response) == string(WebSocket::handshake(request)));
    }

    SECTION("Fail") {
        REQUIRE(request.parse("GET /plugins/ws/path/test HTTP/1.1\r\n\r\n"));
        request.client = client;
        REQUIRE_THROWS_MATCHES(router.handle(request), out_of_range, Catch::Message("Route not found"));
        CHECK_FALSE(plugin.isConnected());
    }
}

TEST_CASE("WebSocketPlugin - Send message") {
    Router router;
    Request request;
    Response response;
    shared_ptr client = make_shared<MockClient>();

    request.headers = {
            {"Sec-WebSocket-Key", "secret"}
    };

    WebSocketPlugin plugin(&router);
    CHECK_FALSE(plugin.isConnected());

    REQUIRE(request.parse("GET /plugins/ws/test HTTP/1.1\r\n\r\n"));
    request.client = client;
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
