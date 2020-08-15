//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"
#include "DatabasePlugin.h"

Inspector::Inspector(DatabaseProvider *databaseProvider) {
    server = new HttpServer;

    server->get("/", [](const Request &req, const Params &params) {
        return Response("Server up!");
    });

    databasePlugin = new DatabasePlugin(server, databaseProvider);
    networkPlugin = new NetworkPlugin(server);
    customPlugin = new CustomPlugin(server);
}

thread *Inspector::bind(int port) {
    return server->start(port);
}

void Inspector::setCipherKey(string database, string password, int version) {
    databasePlugin->setCipherKey(database, password, version);
}

bool Inspector::isConnected() const {
    return networkPlugin->isConnected();
}

void Inspector::sendRequest(string uid, string headers, string body) {
    networkPlugin->sendRequest(uid, headers, body);
}

void Inspector::sendResponse(string uid, string headers, string body, bool compressed) {
    networkPlugin->sendResponse(uid, headers, body, compressed);
}

void Inspector::addPlugin(string key, string name, PluginAction action) {
    customPlugin->addPlugin(key, name, action);
}
