//
// Created by Paker on 2019-10-23.
//

#include "Inspector.h"

Inspector::Inspector(std::shared_ptr<DatabaseProvider> databaseProvider, DeviceInfo info)
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
    for (std::thread *th : threads) {
        th->join();
    }
}

void Inspector::setCipherKey(const std::string &database, const std::string &password, int version) {
    databasePlugin.setCipherKey(database, password, version);
}

bool Inspector::isConnected() const {
    return networkPlugin.isConnected();
}

void Inspector::sendRequest(const std::string &uid, const std::string &headers, const std::string &body) {
    std::thread([=, this] {
        networkPlugin.sendRequest(uid, headers, body);
    }).detach();
}

void Inspector::sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool is_compressed) {
    std::thread([=, this] {
        networkPlugin.sendResponse(uid, headers, body, is_compressed);
    }).detach();
}

void Inspector::addPlugin(const std::string &key, const std::string &name, const PluginAction &action) {
    customPlugin.addPlugin(key, name, action);
}

void Inspector::addLivePlugin(const std::string &key, const std::string &name, const PluginAction &action) {
    customPlugin.addLivePlugin(key, name, action);
}

void Inspector::addPluginAPI(const std::string &method, const std::string &path, const PluginAPIAction &action) {
    customPlugin.addPluginAPI(method, path, action);
}

void Inspector::sendMessage(const std::string &key, const std::string &message) {
    std::thread([=, this] {
        webSocketPlugin.sendMessage(key, message);
    }).detach();
}
