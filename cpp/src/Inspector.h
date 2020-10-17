//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_INSPECTOR_H
#define INSPECTOR_INSPECTOR_H

#include "HttpServer.h"
#include "DatabasePlugin.h"
#include "NetworkPlugin.h"
#include "CustomPlugin.h"
#include "WebSocketPlugin.h"
#include "Broadcaster.h"

class Inspector {
private:
    DeviceInfo info;
    HttpServer server;
    Broadcaster broadcaster;
    std::vector<std::thread*> threads;
    std::unique_ptr<NetworkPlugin> networkPlugin;
    std::unique_ptr<DatabasePlugin> databasePlugin;
    std::unique_ptr<CustomPlugin> customPlugin;
    std::unique_ptr<WebSocketPlugin> webSocketPlugin;

public:
    Inspector(std::shared_ptr<DatabaseProvider> databaseProvider, const DeviceInfo &info);

    void bind(int port);

    void setCipherKey(const std::string &database, const std::string &password, int version);

    bool isConnected() const;

    void sendRequest(const std::string &uid, const std::string &headers, const std::string &body);

    void sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool compressed = false);

    void addPlugin(const std::string &key, const std::string &name, PluginAction action);

    void addLivePlugin(const std::string &key, const std::string &name, PluginAction action);

    void addPluginAPI(const std::string &method, const std::string &path, PluginAPIAction action);

    void sendMessage(const std::string &key, const std::string &message);

    void stop();
};

#endif //INSPECTOR_INSPECTOR_H
