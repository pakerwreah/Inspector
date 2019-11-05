//
// Created by Paker on 29/10/19.
//

#include "DatabasePlugin.h"
#include "Database.h"
#include "HttpServer.h"
#include "util.h"

using namespace util;

DatabasePlugin::DatabasePlugin(HttpServer *server, DatabaseProvider *_provider) {
    this->provider = _provider;

    server->get("/database/list", [this](Request req) {
        int index = 0;
        auto paths = provider->databaseList();
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

    server->put("/database/current/{index}", [this](Request req) {
        try {
            auto body = req.body;
            auto index = stoi(req.params["index"]);

            selectDB(index);

            Database db(db_path);
        } catch (out_of_range &ex) {
            return Response(ex.what(), 400);
        } catch (exception &ex) {
            return Response(ex.what(), 500);
        }
        return Response(db_path);
    });

    server->post("/database/query", [this](Request req) {
        auto sql = req.body;

        try {
            Database db(db_path);

            auto start = timestamp();

            auto res = db.query(sql);

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

    server->post("/database/execute", [this](Request req) {
        try {
            Database db(db_path);

            auto start = timestamp();

            db.transaction();
            db.execute(req.body);
            db.commit();

            auto duration = benchmark(start);

            json data = {{"duration", duration}};

            return Response(data);
        } catch (exception &ex) {
            return Response(ex.what(), 400);
        }
    });
}

void DatabasePlugin::selectDB(int index) {
    auto list = provider->databaseList();
    if (index >= 0 && index < list.size()) {
        db_path = list[index];
    } else {
        throw out_of_range("Index out of range");
    }
}