#include "catch.hpp"
#include "Broadcaster.h"
#include "UDPSocket.h"

using namespace std;

const int test_port = 50000;

TEST_CASE("Broadcaster - Success") {
    Broadcaster broadcaster;
    broadcaster.setInterval(10ms);
    CHECK_FALSE(broadcaster.broadcasting());
    thread *th = broadcaster.start(test_port, {});
    this_thread::sleep_for(1ms);
    CHECK(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK_FALSE(broadcaster.broadcasting());
    CHECK_FALSE(broadcaster.error());
}

TEST_CASE("Broadcaster - Fail") {
    Broadcaster broadcaster;
    thread *th = broadcaster.start(0, {});
    broadcaster.stop();
    th->join();
    CHECK_FALSE(broadcaster.broadcasting());
    CHECK((broadcaster.error() == EADDRNOTAVAIL || broadcaster.error() == EINVAL));
}

TEST_CASE("Broadcaster - Invalid port") {
    Broadcaster broadcaster;
    thread *th = broadcaster.start(-1, {});
    broadcaster.stop();
    th->join();
    CHECK_FALSE(broadcaster.broadcasting());
}
