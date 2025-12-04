#include "catch.hpp"
#include "compress.hpp"
#include "HttpServer.h"
#include "SocketClient.h"
#include "MockClient.h"
#include "MockHttpServer.h"
#include <thread>

using namespace std::chrono_literals;

void operator<<(std::string &lhs, const Request &request) {
    lhs = request.str();
}

bool operator==(const std::string &lhs, const Response &response) {
    return lhs == response.str();
}

static constexpr int test_port = 50000;

TEST_CASE_METHOD(HttpServer, "HttpServer - Invalid request") {
    auto client = std::make_shared<MockClient>();
    REQUIRE_NOTHROW(process(client));
    CHECK(client->sent == Response::BadRequest());
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route not found") {
    auto client = std::make_shared<MockClient>();
    client->recv << Request {.method = "GET", .path = "/test/path"};
    REQUIRE_NOTHROW(process(client));
    CHECK(client->sent == Response::NotFound("Route not found"));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Internal error") {
    router.get("/test/path", [](const Request &, const Params &) -> Response {
        throw std::runtime_error("Internal error");
    });
    auto client = std::make_shared<MockClient>();
    client->recv << Request {.method = "GET", .path = "/test/path"};
    REQUIRE_NOTHROW(process(client));
    CHECK(client->sent == Response::InternalError("Internal error"));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route found") {
    auto client = std::make_shared<MockClient>();
    Request request {.method = "GET", .path = "/test/path"};
    Response expected("response data");

    router.get("/test/path", [expected](const Request &, const Params &) {
        return expected;
    });

    SECTION("Response plain") {
        client->recv << request;
        REQUIRE_NOTHROW(process(client));
        CHECK(client->sent == expected);
    }

    SECTION("Response compressed") {
        expected.headers["Content-Encoding"] = "gzip";
        expected.body = gzip::compress(expected.body.c_str(), expected.body.size());
        request.headers["Accept-Encoding"] = "gzip, deflate";
        client->recv << request;
        REQUIRE_NOTHROW(process(client));
        CHECK(client->sent == expected);
    }
}

TEST_CASE_METHOD(HttpServer, "HttpServer - CORS") {
    auto client = std::make_shared<MockClient>();

    client->recv << Request {.method = "OPTIONS", .path = "/path"};
    REQUIRE_NOTHROW(process(client));

    Response response;
    response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, PATCH, DELETE";

    CHECK(client->sent == response);
}

TEST_CASE("HttpServer - Start/Stop") {
    HttpServer server;
    CHECK_FALSE(server.listening());
    std::thread *th = server.start(test_port);
    std::this_thread::sleep_for(1ms);
    CHECK(server.listening());
    REQUIRE(server.stop());
    th->join();
    CHECK_FALSE(server.listening());
    CHECK_FALSE(server.error());
}

TEST_CASE("HttpServer - Fail") {
    Socket rogue;
    CHECK(rogue.create());
    CHECK(rogue.bind(test_port));
    CHECK(rogue.listen());

    HttpServer server;
    server.setReconnectInterval(10ms);
    std::thread *th = server.start(test_port);
    std::this_thread::sleep_for(1ms);
    CHECK_FALSE(server.listening());
    REQUIRE(server.stop());
    th->join();
    CHECK(server.error() == EADDRINUSE);
}

TEST_CASE("HttpServer - Accept Success") {
    MockHttpServer server;
    server.processor = [](std::shared_ptr<Client> client) {
        (void)client->send("hello");
    };
    std::thread *th = server.start(test_port);
    std::this_thread::sleep_for(1ms);
    CHECK(server.listening());

    auto client = std::make_unique<Socket>();
    CHECK(client->create());
    CHECK(client->connect("localhost", test_port));

    SocketClient socketClient(std::move(client));
    CHECK(socketClient.read() == "hello");

    REQUIRE(server.stop());
    th->join();
    CHECK_FALSE(server.error());
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Accept Fail") {
    setReconnectInterval(10ms);
    std::thread *th = start(test_port);
    std::this_thread::sleep_for(1ms);

    // simulate network failure
    int m_sock = server.fd();
    shutdown(m_sock, SHUT_RDWR);
    close(m_sock);
    std::this_thread::sleep_for(1ms);

    stop();
    th->join();
    CHECK(error() == EBADF);
}
