//
// Created by Paker on 26/10/19.
//

#include "Inspector.h"
#include "util.h"
#include <unistd.h>
#include <thread>
#include <vector>
#include <sys/time.h>
#include <sstream>

using namespace std;

class TestDatabaseProvider : public DatabaseProvider {
protected:
    vector<string> databasePathList() override {
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
            string uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br\nMethod: GET\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 200\nContent-Type: text/plain\nContent-Length: 13\n", "Hello client!");
            rsleep();
        }
    });
    new thread([&] {
        while (true) {
            sleep(2);
            string uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/post\nMethod: POST\n", "Some content");
            rsleep();
            string resp = json{{"foo", "bar"}}.dump();
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
            string uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/put\nMethod: PUT\n", "Test");
            rsleep();
            inspector.sendResponse(uid, "Status: 400\nContent-Type: text/html\nContent-Length: 7\n", "Not OK!");
            rsleep();
        }
    });
    new thread([&] {
        while (true) {
            sleep(4);
            string uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/delete\nMethod: DELETE\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 200\nContent-Type: text/plain\nContent-Length: 8\n", "Deleted!");
            rsleep();
        }
    });
    new thread([&] {
        while (true) {
            sleep(5);
            string uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.google.com.br/patch\nMethod: PATCH\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 500\nContent-Type: text/plain\nContent-Length: 22\n",
                                   "Internal Server Error!");
        }
    });
    new thread([&] {
        while (true) {
            sleep(6);
            string uid = get_uid();
            inspector.sendRequest(uid, "URL: http://www.terra.com.br/redirect\nMethod: GET\n", "");
            rsleep();
            inspector.sendResponse(uid, "Status: 301\nContent-Type: text/plain\nContent-Length: 11\n", "Redirected!");
        }
    });
}

int main() {

    Inspector inspector(new TestDatabaseProvider);

    inspector.setCipherKey("database_cipher3.db", "123456", 3);
    inspector.setCipherKey("database_cipher4.db", "1234567", 4);

    thread *th = inspector.bind(30000);

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
        ostringstream os;
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

    json tree;
    json::array_t files{
            "File 1",
            "File 2",
            "File 3"
    };
    for (int i = 1; i <= 5; i++) {
        tree["root"]["Folder " + to_string(i)] = {
                {"Folder A" + to_string(i), files},
                {"Folder B" + to_string(i), {}},
                {"Folder C" + to_string(i), {}}
        };
    }

    inspector.addPlugin("explorer", "Explorer", [] {
        string uid = get_uid(); // anti-cache
        string host = "http://localhost:30000";
        string api = "/plugins/api/filesystem/list";

        ostringstream os;
        os << "<div class='absolute-expand panel d-flex flex-column'>";
        os << "     <h3 class='accent--text pa-3 text-center'>Explorer Plugin</h3>";
        os << "     <iframe class='flex' style='border:none' src='" + host + api + "?path=/&" + uid + "'></iframe>";
        os << "</div>";
        return os.str();
    });

    inspector.addPluginAPI("GET", "filesystem/list", [tree](const Params &params) {
        const string uid = get_uid(); // anti-cache
        const string path = util::trim(params.at("path"), "/");
        ostringstream os;
        os << R"(<style>
                    html { padding: 1.5rem; }
                    html * { color: #888; text-decoration: none; }
                    h4, h5 * { margin-left: 1.5rem; color: goldenrod; }
                </style>)";

        os << "<h4>/" + path + "</h4>";

        json root = tree.at("root");
        auto parts = util::split(path, '/', false);
        for (string part : parts) {
            if (root.is_object()) {
                root = root.at(part);
            }
        }
        auto makeLink = [&uid](const string &action, const string &path, const string &name) {
            return "<a href='/plugins/api/filesystem/" + action +
                   "?path=/" + util::trim(path, "/") + "&" + uid + "'>" + name +
                   "</a>";
        };

        if (parts.size()) {
            parts.pop_back();
            string back = util::join(parts, '/');
            os << "<h5>" + makeLink("list", back, "&larr; Go back") + "</h5>";
        }

        os << "<ul>";
        for (auto item : root.items()) {
            bool isFile = item.value().is_string();
            string name = isFile ? item.value().get<string>() : item.key();
            string action = isFile ? "open" : "list";
            os << "<li>" + makeLink(action, path + "/" + name, name) + "</li>";
        }
        os << "</ul>";
        return os.str();
    });

    // TODO: Implement "filesystem/open"
    // TODO: Explore real files

    th->join();

    return 0;
}