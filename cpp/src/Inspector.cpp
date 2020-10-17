//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"

using namespace std;

Inspector::Inspector(shared_ptr<DatabaseProvider> databaseProvider, const DeviceInfo &info) : info(info) {
    databasePlugin = make_unique<DatabasePlugin>(server.router, databaseProvider);
    networkPlugin = make_unique<NetworkPlugin>(server.router);
    customPlugin = make_unique<CustomPlugin>(server.router);
    webSocketPlugin = make_unique<WebSocketPlugin>(server.router);
}

void Inspector::bind(int port) {
    threads.push_back(server.start(port));
    threads.push_back(broadcaster.start(port, info));
}

void Inspector::stop() {
    broadcaster.stop();
    server.stop();
    for (thread *th : threads) {
        th->join();
    }
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
        webSocketPlugin->sendMessage(key, message);
    }).detach();
}
