//
// Created by Paker on 29/10/19.
//

#include "DatabasePlugin.h"
#include "Database.h"
#include "HttpServer.h"
#include "util.h"
#include <thread>
#include <unistd.h>

using namespace util;

static int close_token;

shared_ptr<Database> DatabasePlugin::open() {
    if (db_path.empty()) {
        try {
            selectDB(0);
        } catch (out_of_range &ex) {
            throw runtime_error("No databases available");
        }
    }

    if (!db_con) {
        auto name = split(db_path, '/').back();
        if (cipher.count(name)) {
            SQLCipher config = cipher[name];
            db_con = make_shared<Database>(db_path, config.password, config.version);
        } else {
            db_con = make_shared<Database>(db_path);
        }
    }

    thread([this]() {
        auto token = ++close_token;
        sleep(5);
        if (token == close_token) {
            db_con = nullptr;
        }
    }).detach();

    return db_con;
}

DatabasePlugin::DatabasePlugin(HttpServer *server, DatabaseProvider *_provider) {
    this->provider = _provider;

    server->get("/database/list", [this](const Request &, const Params &) {
        int index = 0;
        auto paths = databasePathList();
        auto names = json::array();
        for (int i = 0; i < paths.size(); i++) {
            names += split(paths[i], '/').back();
            if (paths[i] == db_path) {
                index = i;
            }
        }

        json data = {{"databases", names},
                     {"current",   index}};

        return Response(data);
    });

    server->put("/database/current/{index}", [this](const Request &request, const Params &params) {
        try {
            auto body = request.body;
            auto index = stoi(params.at("index"));

            selectDB(index);

            open();
        } catch (out_of_range &ex) {
            return Response(ex.what(), 400);
        } catch (exception &ex) {
            return Response(ex.what(), 500);
        }
        return Response(db_path);
    });

    server->post("/database/query", [this](const Request &request, const Params &) {
        auto sql = request.body;

        try {
            auto db = open();

            auto start = timestamp();

            auto res = db->query(sql);

            auto duration = benchmark(start);

            auto headers = res.headers();

            auto cols = headers.size();

            auto rows = json::array();

            while (res.next()) {
                json row;
                for (int i = 0; i < cols; i++) {
                    switch (res.type(i)) {
                        case SQLITE_INTEGER:
                            row += res.integer(i);
                            break;
                        case SQLITE_FLOAT:
                            row += res.decimal(i);
                            break;
                        case SQLITE_NULL:
                            row += "NULL";
                            break;
                        default:
                            row += res.text(i);
                            break;
                    }
                }
                rows += row;
            }

            json data = {{"headers",  headers},
                         {"data",     rows},
                         {"duration", duration}};

            return Response(data);
        } catch (exception &ex) {
            return Response(ex.what(), 400);
        }
    });

    server->post("/database/execute", [this](const Request &request, const Params &) {
        try {
            auto db = open();

            auto start = timestamp();

            db->transaction();
            db->execute(request.body);
            db->commit();

            auto duration = benchmark(start);

            json data = {{"duration", duration}};

            return Response(data);
        } catch (exception &ex) {
            return Response(ex.what(), 400);
        }
    });
}

vector<string> DatabasePlugin::databasePathList() {
    return filter(provider->databasePathList(), [](const string &item) { return !endsWith(item, "-journal"); });
}

void DatabasePlugin::selectDB(int index) {
    auto list = databasePathList();
    if (index >= 0 && index < list.size()) {
        db_path = list[index];
        db_con = nullptr;
    } else {
        throw out_of_range("Index out of range");
    }
}

void DatabasePlugin::setCipherKey(const string &database, const string &password, int version) {
    cipher[database] = {password, version};
}
