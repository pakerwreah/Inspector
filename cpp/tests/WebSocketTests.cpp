#include "catch.hpp"
#include "WebSocket.h"
#include "MockClient.h"
#include "util.h"

using namespace std;

TEST_CASE("WebSocket - Send") {
    shared_ptr client = make_shared<MockClient>();
    WebSocket ws(client);
    const string data1 = "Lorem ipsum dolor sit amet";
    const string data2 = string(126, '.');
    const string data3 = string(65536, '.');

    SECTION("Text") {
        ws.send(data1, false);
        CHECK(client->sent == "\x81\x1A" + data1);

        ws.send(data2, false);
        CHECK(util::startsWith(client->sent, "\x81"));
        CHECK(client->sent == WebSocket::pack(data2, false));

        ws.send(data3, false);
        CHECK(util::startsWith(client->sent, "\x81"));
        CHECK(client->sent == WebSocket::pack(data3, false));
    }

    SECTION("Binary") {
        ws.send(data1, true);
        CHECK(client->sent == "\x82\x1A" + data1);

        ws.send(data2, true);
        CHECK(util::startsWith(client->sent, "\x82"));
        CHECK(client->sent == WebSocket::pack(data2, true));

        ws.send(data3, true);
        CHECK(util::startsWith(client->sent, "\x82"));
        CHECK(client->sent == WebSocket::pack(data3, true));
    }
}

TEST_CASE("WebSocket - Handshake Success") {
    Headers expected_headers = {
            {"Connection",           "Upgrade"},
            {"Upgrade",              "websocket"},
            {"Sec-WebSocket-Accept", "I9SptFFOtcFUqaosQ3sgiQpYo3Q="}
    };

    Request request;
    request.headers = {{"Sec-WebSocket-Key", "secret"}};
    Response response = WebSocket::handshake(request);

    CHECK(response.code == 101);
    CHECK(response.headers == expected_headers);
    CHECK(response.body == "");
}

TEST_CASE("WebSocket - Handshake Fail") {
    Request request;
    Response response = WebSocket::handshake(request);
    Response expected = Response::BadRequest("Sec-WebSocket-Key header not found");
    CHECK(string(response) == string(expected));
}
