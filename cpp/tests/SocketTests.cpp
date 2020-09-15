#include "catch.hpp"
#include "SocketClient.h"
#include <thread>
#include <mutex>

using namespace std;

TEST_CASE("SocketClient") {
    const int port = 50000;

    Socket server;
    REQUIRE(server.create());
    REQUIRE(server.bind(port));
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
        CHECK(client->connect("localhost", port));
        m_connect.unlock();
        SocketClient socketClient(move(client));
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
