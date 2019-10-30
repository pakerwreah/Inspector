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
        auto list = provider->databaseList();
        ostringstream os;
        for (auto name : list) {
            os << split(name, '/').back() << endl;
        }
        return Response(os.str());
    });

    server->put("/database/current", [this](Request req) {
        try {
            auto body = req.body;
            auto index = atoi(req.body.c_str());

            selectDB(index);

            Database db(db_path);
        } catch (out_of_range &ex) {
            return Response(ex.what(), 400);
        } catch (exception &ex) {
            return Response(ex.what(), 500);
        }
        return Response("Selected: " + db_path);
    });

    //TODO: retornar dados como JSON
    server->post("/database/query", [this](Request req) {
        auto sql = req.body;

        try {
            Database db(db_path);

            auto res = db.query(sql);

            ostringstream os;

            os << join(res.headers(), ';') << endl;

            int cols = res.headers().size();

            while (res.next()) {
                vector<string> row;
                for (int i = 0; i < cols; i++) {
                    ostringstream val;
                    switch (res.type(i)) {
                        case SQLITE_INTEGER:
                            val << res.integer(i);
                            break;
                        case SQLITE_FLOAT:
                            val << res.decimal(i);
                            break;
                        case SQLITE_NULL:
                            val << "NULL";
                            break;
                        default:
                            val << res.text(i);
                            break;
                    }
                    row.push_back(val.str());
                }
                os << join(row, ';') << endl;
            }

            return Response(os.str());
        } catch (exception &ex) {
            return Response(string(ex.what()) + "\n\n" + sql, 400);
        }
    });

    server->post("/database/execute", [this](Request req) {
        try {
            Database db(db_path);

            db.transaction();
            db.execute(req.body);
            db.commit();

            return Response();
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