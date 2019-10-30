//
// Created by Paker on 29/10/19.
//

#ifndef INSPECTOR_DATABASEPLUGIN_H
#define INSPECTOR_DATABASEPLUGIN_H

#include <string>

class HttpServer;

using namespace std;

class DatabaseProvider {
public:
    virtual vector<string> databaseList() = 0;
};

class DatabasePlugin {
    string db_path;
    DatabaseProvider *provider;
public:
    DatabasePlugin(HttpServer *server, DatabaseProvider *provider);

    void selectDB(int index);
};


#endif //INSPECTOR_DATABASEPLUGIN_H
