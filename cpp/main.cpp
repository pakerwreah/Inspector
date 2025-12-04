//
// Created by Paker on 26/10/19.
//

#include "Inspector.h"
#include "util.h"

#include "ext/explorer/explorer.h"
#include "ext/realtime/realtime.h"
#include "ext/usage/usage.h"

#include <unistd.h>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
using nlohmann::json;

class TestDatabaseProvider : public DatabaseProvider {
protected:
    std::vector<std::string> databasePathList() const override {
        return {"database.db", "database_cipher3.db", "database_cipher4.db"};
    }
};

static void rsleep() {
    static unsigned int seed = 0;
    sleep((rand_r(&seed) % 5) + 5);
}

static void mockNetwork(Inspector &inspector) {
    new std::thread([&] {
        while (true) {
            sleep(1);
            std::string uid = util::uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br\nMethod: GET\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 200\nContent-Type: text/plain\nContent-Length: 13\n", "Hello client!");
            rsleep();
        }
    });
    new std::thread([&] {
        while (true) {
            sleep(2);
            std::string uid = util::uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/post\nMethod: POST\n", "Some content");
            rsleep();
            std::string resp = json{{"foo", "bar"}}.dump();
            inspector.sendResponse(uid,
                                   "Status: 200\nContent-Type: application/json\nContent-Length: "
                                   + std::to_string(resp.size())
                                   + "\n",
                                   resp);
        }
    });
    new std::thread([&] {
        while (true) {
            sleep(3);
            std::string uid = util::uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/put\nMethod: PUT\n", "Test");
            rsleep();
            inspector.sendResponse(uid, "Status: 400\nContent-Type: text/html\nContent-Length: 7\n", "Not OK!");
            rsleep();
        }
    });
    new std::thread([&] {
        while (true) {
            sleep(4);
            std::string uid = util::uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/delete\nMethod: DELETE\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 200\nContent-Type: text/plain\nContent-Length: 8\n", "Deleted!");
            rsleep();
        }
    });
    new std::thread([&] {
        while (true) {
            sleep(5);
            std::string uid = util::uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/patch\nMethod: PATCH\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 500\nContent-Type: text/plain\nContent-Length: 22\n",
                                   "Internal Server Error!");
        }
    });
    new std::thread([&] {
        while (true) {
            sleep(6);
            std::string uid = util::uid();
            inspector.sendRequest(uid, "URL: http://www.terra.com.br/redirect\nMethod: GET\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 301\nContent-Type: text/plain\nContent-Length: 11\n", "Redirected!");
        }
    });
    new std::thread([&] {
        while (true) {
            rsleep();
            inspector.sendMessage("realtime", "Message uid: " + util::uid());
        }
    });
}

int main() {
    DeviceInfo info = {"desktop", "Demo Application", "com.demo.app", "v1.0.0"};
    Inspector inspector(std::make_shared<TestDatabaseProvider>(), info);

    inspector.setCipherKey("database_cipher3.db", "123456", 3);
    inspector.setCipherKey("database_cipher4.db", "1234567", 4);

    inspector.bind(30000);

    mockNetwork(inspector);

    inspector.addPlugin("prefs", "Shared Preferences", [] {
        sleep(1);
        return json{
                {"foo",    "bar"},
                {"number", 123},
                {"array",  {1, 2, 3}}
        }.dump();
    });

    inspector.addPlugin("lorem-ipsum", "Lorem ipsum", [] {
        sleep(3);
        std::ostringstream os;
        for (int i = 0; i < 10; i++) {
            os << R"(
                <span class='accent--text'>
                    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
                    Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
                    Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.
                    Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
                </span>
            )";
        }
        return os.str();
    });

    [[maybe_unused]] Explorer explorer(inspector, "../../");

    [[maybe_unused]] Realtime realtime(inspector);

    [[maybe_unused]] Usage usage(inspector);

    std::this_thread::sleep_for(100h);

    return 0;
}
