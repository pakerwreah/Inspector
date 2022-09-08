#include "catch.hpp"
#include "SocketClient.h"
#include "UDPSocket.h"
#include <thread>
#include <mutex>

using namespace std;

const int test_port = 50000;
const timeval timeout = {0, 1000};

TEST_CASE("TCP Socket") {
    Socket server;
    REQUIRE(server.create());
    REQUIRE(server.bind(test_port));
    REQUIRE(server.listen());
    server.set_non_blocking();

    mutex m_connect, m_read, m_check;
    m_connect.lock();
    m_read.lock();
    m_check.lock();
    string response;

    thread th([&] {
        unique_ptr client = make_unique<Socket>();
        CHECK(client->create());
        CHECK(client->connect("localhost", test_port));
        m_connect.unlock();
        SocketClient socketClient(std::move(client));
        m_read.lock(); // wait for server signal
        response = socketClient.read();
        m_check.unlock(); // signal server to check for response
    });

    m_connect.lock();
    Socket *client = nullptr;
    CHECK(server.accept(client));
    REQUIRE(client != nullptr);

    SocketClient socketClient((unique_ptr<Socket>(client)));
    const string msg = "Test data";
    CHECK(socketClient.send(msg));
    m_read.unlock(); // signal thread to start reading
    m_check.lock(); // wait for response
    CHECK(response == msg);

    th.join();
}

TEST_CASE("UDP Socket") {
    UDPSocket socket;
    CHECK(socket.create());
    CHECK(socket.bind(test_port));
    CHECK(socket.broadcast("hello world!", timeout));
    CHECK(socket.close());
}

TEST_CASE("TCP Socket - Invalid port") {
    Socket socket;
    REQUIRE(socket.create());
    CHECK_FALSE(socket.bind(-1));
    CHECK(errno == EINVAL);
}

TEST_CASE("UDP Socket - Invalid port") {
    UDPSocket socket;
    REQUIRE(socket.create());
    CHECK_FALSE(socket.bind(-1));
    CHECK(errno == EINVAL);
}
