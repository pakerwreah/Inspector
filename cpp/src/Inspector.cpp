//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"

using namespace std;

Inspector::Inspector(shared_ptr<DatabaseProvider> databaseProvider, DeviceInfo info)
    : info(std::move(info))
    , networkPlugin(NetworkPlugin(server.router))
    , databasePlugin(DatabasePlugin(server.router, std::move(databaseProvider)))
    , customPlugin(CustomPlugin(server.router))
    , webSocketPlugin(WebSocketPlugin(server.router)) {}

void Inspector::bind(int port) {
    threads.push_back(server.start(port));
    threads.push_back(broadcaster.start(port, info, {3, 0}));
}

void Inspector::stop() {
    broadcaster.stop();
    server.stop();
    for (thread *th : threads) {
        th->join();
    }
}

void Inspector::setCipherKey(const string &database, const string &password, int version) {
    databasePlugin.setCipherKey(database, password, version);
}

bool Inspector::isConnected() const {
    return networkPlugin.isConnected();
}

void Inspector::sendRequest(const string &uid, const string &headers, const string &body) {
    thread([=, this] {
        networkPlugin.sendRequest(uid, headers, body);
    }).detach();
}

void Inspector::sendResponse(const string &uid, const string &headers, const string &body, bool is_compressed) {
    thread([=, this] {
        networkPlugin.sendResponse(uid, headers, body, is_compressed);
    }).detach();
}

void Inspector::addPlugin(const string &key, const string &name, const PluginAction &action) {
    customPlugin.addPlugin(key, name, action);
}

void Inspector::addLivePlugin(const string &key, const string &name, const PluginAction &action) {
    customPlugin.addLivePlugin(key, name, action);
}

void Inspector::addPluginAPI(const string &method, const string &path, const PluginAPIAction &action) {
    customPlugin.addPluginAPI(method, path, action);
}

void Inspector::sendMessage(const string &key, const string &message) {
    thread([=, this] {
        webSocketPlugin.sendMessage(key, message);
    }).detach();
}
