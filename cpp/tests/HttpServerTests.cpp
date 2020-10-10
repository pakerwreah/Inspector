#include "catch.hpp"
#include "compress.hpp"
#include "HttpServer.h"
#include "SocketClient.h"
#include "MockClient.h"
#include "MockHttpServer.h"

using namespace std;

static const int test_port = 50000;

TEST_CASE_METHOD(HttpServer, "HttpServer - Invalid request") {
    shared_ptr client = make_shared<MockClient>();
    REQUIRE_NOTHROW(process(client));
    CHECK(client->sent == string(Response::BadRequest()));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route not found") {
    shared_ptr client = make_shared<MockClient>();
    client->recv = Request("GET", "/test/path");
    REQUIRE_NOTHROW(process(client));
    CHECK(client->sent == string(Response::NotFound("Route not found")));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Internal error") {
    router.get("/test/path", [](const Request &, const Params &) {
        return throw runtime_error("Internal error"), "";
    });
    shared_ptr client = make_shared<MockClient>();
    client->recv = Request("GET", "/test/path");
    REQUIRE_NOTHROW(process(client));
    CHECK(client->sent == string(Response::InternalError("Internal error")));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route found") {
    shared_ptr client = make_shared<MockClient>();
    Request request("GET", "/test/path");
    Response expected("response data");

    router.get("/test/path", [expected](const Request &, const Params &) {
        return expected;
    });

    SECTION("Response plain") {
        client->recv = request;
        REQUIRE_NOTHROW(process(client));
        CHECK(client->sent == string(expected));
    }

    SECTION("Response compressed") {
        expected.headers["Content-Encoding"] = "gzip";
        expected.body = gzip::compress(expected.body.c_str(), expected.body.size());
        request.headers["Accept-Encoding"] = "gzip, deflate";
        client->recv = request;
        REQUIRE_NOTHROW(process(client));
        CHECK(client->sent == string(expected));
    }
}

TEST_CASE_METHOD(HttpServer, "HttpServer - CORS") {
    Request request("OPTIONS", "/path");
    shared_ptr client = make_shared<MockClient>();

    client->recv = request;
    REQUIRE_NOTHROW(process(client));

    Response response;
    response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, PATCH, DELETE";

    CHECK(client->sent == string(response));
}

TEST_CASE("HttpServer - Start/Stop") {
    HttpServer server;
    CHECK_FALSE(server.listening());
    thread *th = server.start(test_port);
    this_thread::sleep_for(1ms);
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
    thread *th = server.start(test_port);
    this_thread::sleep_for(1ms);
    CHECK_FALSE(server.listening());
    REQUIRE(server.stop());
    th->join();
    CHECK(server.error() == EADDRINUSE);
}

TEST_CASE("HttpServer - Accept Success") {
    MockHttpServer server;
    server.processor = [](shared_ptr<Client> client) {
        client->send("hello");
    };
    thread *th = server.start(test_port);
    this_thread::sleep_for(1ms);
    CHECK(server.listening());

    unique_ptr client = make_unique<Socket>();
    CHECK(client->create());
    CHECK(client->connect("localhost", test_port));

    SocketClient socketClient(move(client));
    CHECK(socketClient.read() == "hello");

    REQUIRE(server.stop());
    th->join();
    CHECK_FALSE(server.error());
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Accept Fail") {
    setReconnectInterval(10ms);
    thread *th = start(test_port);
    this_thread::sleep_for(1ms);

    // simulate network failure
    int m_sock = server.fd();
    shutdown(m_sock, SHUT_RDWR);
    close(m_sock);
    this_thread::sleep_for(1ms);

    stop();
    th->join();
    CHECK(error() == EBADF);
}
