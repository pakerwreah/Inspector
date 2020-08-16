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
    HttpServer *server;
    NetworkPlugin *networkPlugin;
    DatabasePlugin *databasePlugin;
    CustomPlugin *customPlugin;

public:
    Inspector(DatabaseProvider *databaseProvider);

    thread *bind(int port);

    void setCipherKey(const string &database, const string &password, int version);

    bool isConnected() const;

    void sendRequest(const string &uid, const string &headers, const string &body);

    void sendResponse(const string &uid, const string &headers, const string &body, bool compressed = false);

    void addPlugin(const string &key, const string &name, PluginAction action);

    void addPluginAPI(const string &method, const string &path, PluginAPIAction action);

    void addLivePlugin(const string &key, const string &name, const string &filepath);

    void addLivePlugin(const string &key, const string &name, PluginAction action);
};


#endif //INSPECTOR_INSPECTOR_H
