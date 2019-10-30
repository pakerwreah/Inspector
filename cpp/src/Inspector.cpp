//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"
#include "DatabasePlugin.h"

thread *Inspector::bind(int port) {
    return server->start(port);
}

void Inspector::preselectDB() {
    new thread([this] {
        databasePlugin->selectDB(0);
    });
}

Inspector::Inspector() : DatabaseProvider() {
    server = new HttpServer;

    server->get("/", [](Request req) {
        return Response("Server up!");
    });

    databasePlugin = new DatabasePlugin(server, this);
}