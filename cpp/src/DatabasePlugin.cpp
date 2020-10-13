//
// Created by Paker on 29/10/19.
//

#include "DatabasePlugin.h"
#include "util.h"
#include <thread>

using namespace std;
using namespace chrono_literals;
using json = nlohmann::json;

DatabasePlugin::~DatabasePlugin() {
    db_con = nullptr;
}

shared_ptr<Database> DatabasePlugin::open() {
    if (db_path.empty()) {
        try {
            selectDB(0);
        } catch (const out_of_range &) {
            throw runtime_error("No databases available");
        }
    }

    if (!db_con) {
        const string &name = databaseName();
        const auto &it = db_meta.find(name);
        if (it != db_meta.end()) {
            DatabaseMeta config = it->second;
            db_con = make_shared<Database>(db_path, config.password, config.version);
        } else {
            db_con = make_shared<Database>(db_path);
        }
    }

    // debouncer to auto close db after a while
    // it won't abort any queries because it uses shared_ptr
    int token = ++auto_close_token;
    thread([this, token]() {
        this_thread::sleep_for(debounce);
        if (token == auto_close_token) {
            db_con = nullptr;
        }
    }).detach();

    return db_con;
}

DatabasePlugin::DatabasePlugin(Router *router, DatabaseProvider *provider) : provider(provider), debounce(5s) {
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

        return data;
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
            return Response::BadRequest(ex.what());
        }
    });

    router->post("/database/execute", [this](const Request &request, const Params &) {
        shared_ptr<Database> db;
        try {
            db = open();

            auto start = util::timestamp();

            db->transaction();
            db->execute(request.body);
            db->commit();

            auto duration = util::benchmark(start);

            json data = {{"duration", duration}};

            return Response(data);
        } catch (const exception &ex) {
            db->rollback();
            return Response::BadRequest(ex.what());
        }
    });
}

vector<string> DatabasePlugin::databasePathList() {
    const array<string, 3> arr = {"-journal", "-shm", "-wal"};
    return util::filter(provider->databasePathList(), [&arr](const string &item) {
        return none_of(arr.begin(), arr.end(), [&item](const string &suffix) {
            return util::endsWith(item, suffix);
        });
    });
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

string DatabasePlugin::databaseName() const {
    return util::split(db_path, '/').back();
}

bool DatabasePlugin::isOpen() const {
    return db_con != nullptr;
}

void DatabasePlugin::setDebounce(chrono::nanoseconds debounce) {
    this->debounce = debounce;
}
