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

    void setCipherKey(string database, string password, int version);

    bool isConnected() const;

    void sendRequest(string uid, string headers, string body);

    void sendResponse(string uid, string headers, string body, bool compressed = false);

    void addPlugin(string key, string name, PluginAction action);
};


#endif //INSPECTOR_INSPECTOR_H
