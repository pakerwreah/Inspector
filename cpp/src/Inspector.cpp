//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"
#include "DatabasePlugin.h"

Inspector::Inspector() : DatabaseProvider() {
    server = new HttpServer;

    server->get("/", [](Request req) {
        return Response("Server up!");
    });

    databasePlugin = new DatabasePlugin(server, this);
    networkPlugin = new NetworkPlugin(server);
}

thread *Inspector::bind(int port) {
    return server->start(port);
}

void Inspector::preselectDB() {
    thread([this] {
        databasePlugin->selectDB(0);
    }).detach();
}

void Inspector::sendRequest(string uid, string headers, string body) {
    networkPlugin->sendRequest(uid, headers, body);
}

void Inspector::sendResponse(string uid, string headers, string body, bool compressed) {
    networkPlugin->sendResponse(uid, headers, body, compressed);
}
