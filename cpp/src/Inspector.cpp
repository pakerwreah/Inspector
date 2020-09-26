//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"

using namespace std;

Inspector::Inspector(DatabaseProvider *databaseProvider) {
    databasePlugin = make_unique<DatabasePlugin>(&server.router, databaseProvider);
    networkPlugin = make_unique<NetworkPlugin>(&server.router);
    customPlugin = make_unique<CustomPlugin>(&server.router);
    webSocketPlugin = make_unique<WebSocketPlugin>(&server.router);
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
    thread([=] {
        networkPlugin->sendRequest(uid, headers, body);
    }).detach();
}

void Inspector::sendResponse(const string &uid, const string &headers, const string &body, bool compressed) {
    thread([=] {
        networkPlugin->sendResponse(uid, headers, body, compressed);
    }).detach();
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

void Inspector::sendMessage(const string &key, const string &message) {
    thread([=] {
        return webSocketPlugin->sendMessage(key, message);
    }).detach();
}
