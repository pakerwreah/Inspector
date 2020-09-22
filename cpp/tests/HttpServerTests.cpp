#include "catch.hpp"
#include "compress.hpp"
#include "HttpServer.h"
#include "MockClient.h"
#include "SocketClient.h"
#include "MockHttpServer.h"

using namespace std;

TEST_CASE_METHOD(HttpServer, "HttpServer - Invalid request") {
    shared_ptr client = make_shared<MockClient>();
    REQUIRE_NOTHROW(process(client));
    CHECK(client->response == string(Response::BadRequest()));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route not found") {
    shared_ptr client = make_shared<MockClient>();
    client->request = "GET /test/path HTTP/1.1\r\n\r\n";
    REQUIRE_NOTHROW(process(client));
    CHECK(client->response == string(Response::NotFound("Route not found")));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Internal error") {
    router.get("/test/path", [](const Request &, const Params &) {
        return throw runtime_error("Internal error"), "";
    });
    shared_ptr client = make_shared<MockClient>();
    client->request = "GET /test/path HTTP/1.1\r\n\r\n";
    REQUIRE_NOTHROW(process(client));
    CHECK(client->response == string(Response::InternalError("Internal error")));
}

TEST_CASE_METHOD(HttpServer, "HttpServer - Route found") {
    shared_ptr client = make_shared<MockClient>();
    Response expected("response data");

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

TEST_CASE("HttpServer - Start/Stop") {
    const int port = 50000;
    HttpServer server;
    thread *th = server.start(port);
    usleep(1000);
    CHECK(server.listening());
    REQUIRE(server.stop());
    th->join();
    CHECK_FALSE(server.listening());
}

TEST_CASE("HttpServer - Fail") {
    const int port = 50000;

    Socket rogue;
    rogue.create();
    rogue.bind(port);

    HttpServer server;
    thread *th = server.start(port);
    usleep(1000);
    CHECK_FALSE(server.listening());
    REQUIRE(server.stop());
    th->join();
}

TEST_CASE("HttpServer - Accept") {
    const int port = 50000;

    MockHttpServer server;
    server.processor = [](shared_ptr<Client> client) {
        client->send("hello");
    };
    thread *th = server.start(port);

    unique_ptr client = make_unique<Socket>();
    CHECK(client->create());
    CHECK(client->connect("localhost", port));

    SocketClient socketClient(move(client));
    CHECK(socketClient.read() == "hello");

    REQUIRE(server.stop());
    th->join();
}

TEST_CASE("HttpServer - CORS") {
    const int port = 50000;

    HttpServer server;
    thread *th = server.start(port);

    unique_ptr client = make_unique<Socket>();
    CHECK(client->create());
    CHECK(client->connect("localhost", port));

    SocketClient socketClient(move(client));
    CHECK(socketClient.send("OPTIONS /path HTTP/1.1\r\n\r\n"));

    string expected_response = "HTTP/1.1 200 \r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 0\r\n\r\n";

    CHECK(socketClient.read({1, 0}) == expected_response);

    REQUIRE(server.stop());
    th->join();
}
