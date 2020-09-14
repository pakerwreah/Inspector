#include "catch.hpp"
#include "SocketClient.h"
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono_literals;

TEST_CASE("Socket - Timeout") {
    const int port = 50001;
    Socket *accepted_client = nullptr;
    bool should_accept;

    thread ths([&] {
        Socket server;
        server.create();
        server.bind(port);
        server.listen();
        this_thread::sleep_for(1ms);
        if (should_accept) {
            server.accept(accepted_client);
        }
    });

    mutex guard;
    guard.lock();

    SECTION("Success") {
        should_accept = true;
        thread th([&] {
            unique_ptr client = make_unique<Socket>();
            client->create();
            client->connect("localhost", port, {0, 1500});
            guard.unlock();
        });
        guard.lock();
        REQUIRE(accepted_client != nullptr);
        CHECK(accepted_client->is_valid());
        th.join();
    }

    SECTION("Fail") {
        should_accept = false;
        thread th([&] {
            unique_ptr client = make_unique<Socket>();
            client->create();
            client->connect("localhost", port, {0, 500});
            guard.unlock();
        });
        guard.lock();
        CHECK(accepted_client == nullptr);
        th.join();
    }

    ths.join();
}

TEST_CASE("SocketClient - IO") {
    const int port = 50002;

    Socket server;
    REQUIRE(server.create());
    REQUIRE(server.bind(port));
    REQUIRE(server.listen());

    mutex read, check;
    read.lock();
    check.lock();
    string response;

    thread th([&] {
        unique_ptr client = make_unique<Socket>();
        client->create();
        client->connect("localhost", port);
        SocketClient socketClient(move(client));
        read.lock(); // wait for server signal
        response = socketClient.read();
        check.unlock(); // signal server to check for response
    });

    Socket *client = nullptr;
    CHECK(server.accept(client));
    CHECK(client != nullptr);

    SocketClient socketClient((unique_ptr<Socket>(client)));
    const string msg = "Test data";
    CHECK(socketClient.send(msg));
    read.unlock(); // signal thread to start reading
    check.lock(); // wait for response
    CHECK(response == msg);

    th.join();
}
