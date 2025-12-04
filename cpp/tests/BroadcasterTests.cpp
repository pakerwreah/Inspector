#include "catch.hpp"
#include "Broadcaster.h"
#include <thread>

using namespace std::chrono_literals;

const int test_port = 50000;
const timeval timeout = {0, 1000};

TEST_CASE("Broadcaster - Success") {
    Broadcaster broadcaster;
    broadcaster.setInterval(10ms);
    CHECK_FALSE(broadcaster.broadcasting());
    std::thread *th = broadcaster.start(test_port, {}, timeout);
    std::this_thread::sleep_for(2ms);
    CHECK(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK_FALSE(broadcaster.broadcasting());
    CHECK_FALSE(broadcaster.error());
}

TEST_CASE("Broadcaster - Fail") {
    Broadcaster broadcaster;
    std::string name;
    name.resize(1000000);
    std::thread *th = broadcaster.start(test_port, { .name = name }, timeout);
    std::this_thread::sleep_for(2ms);
    CHECK_FALSE(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK(broadcaster.error() == EMSGSIZE);
}

TEST_CASE("Broadcaster - Invalid port") {
    Broadcaster broadcaster;
    std::thread *th = broadcaster.start(-1, {}, timeout);
    std::this_thread::sleep_for(2ms);
    CHECK_FALSE(broadcaster.broadcasting());
    broadcaster.stop();
    th->join();
    CHECK(broadcaster.error() == EINVAL);
}
