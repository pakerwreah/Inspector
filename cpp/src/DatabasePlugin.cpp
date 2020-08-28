//
// Created by Paker on 29/10/19.
//

#include "DatabasePlugin.h"
#include "util.h"
#include <thread>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

shared_ptr<Database> DatabasePlugin::open() {
    if (db_path.empty()) {
        try {
            selectDB(0);
        } catch (const out_of_range &) {
            throw runtime_error("No databases available");
        }
    }

    if (!db_con) {
        auto name = util::split(db_path, '/').back();
        if (db_meta.count(name)) {
            DatabaseMeta config = db_meta[name];
            db_con = make_shared<Database>(db_path, config.password, config.version);
        } else {
            db_con = make_shared<Database>(db_path);
        }
    }

    // debouncer to auto close db after a while
    // it won't abort any queries because it uses shared_ptr
    thread([this]() {
        auto token = ++auto_close_token;
        sleep(5);
        if (token == auto_close_token) {
            db_con = nullptr;
        }
    }).detach();

    return db_con;
}

DatabasePlugin::DatabasePlugin(Router *router, DatabaseProvider *_provider) {
    this->provider = _provider;

    router->get("/database/list", [this](const Request &, const Params &) {
        int index = 0;
        auto paths = databasePathList();
        auto names = json::array();
        for (int i = 0; i < paths.size(); i++) {
            names += util::split(paths[i], '/').back();
            if (paths[i] == db_path) {
                index = i;
            }
        }

        json data = {{"databases", names},
                     {"current",   index}};

        return Response(data);
    });

    router->put("/database/current/{index}", [this](const Request &request, const Params &params) {
        auto index = stoi(params.at("index"));

        selectDB(index);

        open();
        return Response(db_path);
    });

    router->post("/database/query", [this](const Request &request, const Params &) {
        auto sql = request.body;

        try {
            auto db = open();

            auto start = util::timestamp();

            auto res = db->query(sql);

            auto duration = util::benchmark(start);

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
        } catch (const exception &ex) {
            return Response(ex.what(), 400);
        }
    });

    router->post("/database/execute", [this](const Request &request, const Params &) {
        try {
            auto db = open();

            auto start = util::timestamp();

            db->transaction();
            db->execute(request.body);
            db->commit();

            auto duration = util::benchmark(start);

            json data = {{"duration", duration}};

            return Response(data);
        } catch (const exception &ex) {
            return Response(ex.what(), 400);
        }
    });
}

vector<string> DatabasePlugin::databasePathList() {
    return util::filter(provider->databasePathList(),
                        [](const string &item) { return !util::endsWith(item, "-journal"); });
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
    db_meta[database] = {password, version};
}
