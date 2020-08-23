//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"

using namespace std;

Inspector::Inspector(DatabaseProvider *databaseProvider) {
    databasePlugin = make_unique<DatabasePlugin>(&server, databaseProvider);
    networkPlugin = make_unique<NetworkPlugin>(&server);
    customPlugin = make_unique<CustomPlugin>(&server);
}

thread *Inspector::bind(int port) {
    return server.start(port);
}

void Inspector::setCipherKey(const string &database, const string &password, int version) {
    databasePlugin->setCipherKey(database, password, version);
}

bool Inspector::isConnected() const {
    return networkPlugin->isConnected();
}

void Inspector::sendRequest(const string &uid, const string &headers, const string &body) {
    networkPlugin->sendRequest(uid, headers, body);
}

void Inspector::sendResponse(const string &uid, const string &headers, const string &body, bool compressed) {
    networkPlugin->sendResponse(uid, headers, body, compressed);
}

void Inspector::addPlugin(const string &key, const string &name, PluginAction action) {
    customPlugin->addPlugin(key, name, action);
}

void Inspector::addLivePlugin(const string &key, const string &name, PluginAction action) {
    customPlugin->addLivePlugin(key, name, action);
}

void Inspector::addPluginAPI(const string &method, const string &path, PluginAPIAction action) {
    customPlugin->addPluginAPI(method, path, action);
}
