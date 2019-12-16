//
// Created by Paker on 26/10/19.
//

#include "Inspector.h"
#include <unistd.h>
#include <thread>
#include <vector>
#include <sys/time.h>

using namespace std;

class TestInspector : public Inspector {
protected:
    vector<string> databaseList() override {
        return {"database.db", "database_cipher3.db", "database_cipher4.db"};
    }
};

static string get_uid() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return to_string(tp.tv_sec) + "-" + to_string(tp.tv_usec);
}

static void rsleep() {
    srand(time(nullptr)); // use current time as seed for random generator
    sleep((rand() % 5) + 5);
}

static void mockNetwork(Inspector &inspector) {
    new thread([&] {
        while (true) {
            sleep(1);
            auto uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br\nMethod: GET\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 200\nContent-Type: text/plain\nContent-Length: 13\n", "Hello client!");
            rsleep();
        }
    });
    new thread([&] {
        while (true) {
            sleep(2);
            auto uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/post\nMethod: POST\n", "Some content");
            rsleep();
            auto resp = json{{"foo", "bar"}}.dump();
            inspector.sendResponse(uid,
                                   "Status: 200\nContent-Type: application/json\nContent-Length: "
                                   + to_string(resp.size())
                                   + "\n",
                                   resp);
        }
    });
    new thread([&] {
        while (true) {
            sleep(3);
            auto uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/put\nMethod: PUT\n", "Test");
            rsleep();
            inspector.sendResponse(uid, "Status: 400\nContent-Type: text/html\nContent-Length: 7\n", "Not OK!");
            rsleep();
        }
    });
    new thread([&] {
        while (true) {
            sleep(4);
            auto uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/delete\nMethod: DELETE\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 200\nContent-Type: text/plain\nContent-Length: 8\n", "Deleted!");
            rsleep();
        }
    });
    new thread([&] {
        while (true) {
            sleep(5);
            auto uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/patch\nMethod: PATCH\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 500\nContent-Type: text/plain\nContent-Length: 22\n",
                                   "Internal Server Error!");
        }
    });
    new thread([&] {
        while (true) {
            sleep(6);
            auto uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.terra.com.br/redirect\nMethod: GET\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 301\nContent-Type: text/plain\nContent-Length: 11\n", "Redirected!");
        }
    });
}

int main() {

    TestInspector inspector;

    inspector.setCipherKey("database_cipher3.db", "123456", 3);
    inspector.setCipherKey("database_cipher4.db", "1234567", 4);

    auto th = inspector.bind(30000);

//     mockNetwork(inspector);

    th->join();

    return 0;
}