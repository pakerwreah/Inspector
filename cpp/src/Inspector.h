//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_INSPECTOR_H
#define INSPECTOR_INSPECTOR_H

#include "HttpServer.h"
#include "DatabasePlugin.h"
#include "NetworkPlugin.h"
#include "CustomPlugin.h"

class Inspector {
    HttpServer server;
    std::unique_ptr<NetworkPlugin> networkPlugin;
    std::unique_ptr<DatabasePlugin> databasePlugin;
    std::unique_ptr<CustomPlugin> customPlugin;

public:
    Inspector(DatabaseProvider *databaseProvider);

    std::thread *bind(int port);

    void setCipherKey(const std::string &database, const std::string &password, int version);

    bool isConnected() const;

    void sendRequest(const std::string &uid, const std::string &headers, const std::string &body);

    void sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool compressed = false);

    void addPlugin(const std::string &key, const std::string &name, PluginAction action);

    void addLivePlugin(const std::string &key, const std::string &name, PluginAction action);

    void addPluginAPI(const std::string &method, const std::string &path, PluginAPIAction action);
};


#endif //INSPECTOR_INSPECTOR_H
