#include "catch.hpp"
#include "Broadcaster.h"
#include <thread>

using namespace std;

const int test_port = 50000;
const timeval timeout = {0, 1000};

TEST_CASE("Broadcaster - Success") {
    Broadcaster broadcaster;
    broadcaster.setInterval(10ms);
    CHECK_FALSE(broadcaster.broadcasting());
    thread *th = broadcaster.start(test_port, {}, timeout);
    this_thread::sleep_for(2ms);
    CHECK(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK_FALSE(broadcaster.broadcasting());
    CHECK_FALSE(broadcaster.error());
}

TEST_CASE("Broadcaster - Fail") {
    Broadcaster broadcaster;
    string name;
    name.resize(1000000);
    thread *th = broadcaster.start(test_port, { .name = name }, timeout);
    this_thread::sleep_for(2ms);
    CHECK_FALSE(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK(broadcaster.error() == EMSGSIZE);
}

TEST_CASE("Broadcaster - Invalid port") {
    Broadcaster broadcaster;
    thread *th = broadcaster.start(-1, {}, timeout);
    this_thread::sleep_for(2ms);
    CHECK_FALSE(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK(broadcaster.error() == EINVAL);
}
