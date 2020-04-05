//
// Created by Paker on 29/10/19.
//

#ifndef INSPECTOR_DATABASEPLUGIN_H
#define INSPECTOR_DATABASEPLUGIN_H

#include <map>
#include <string>

class HttpServer;

class Database;

using namespace std;

class DatabaseProvider {
public:
    virtual vector<string> databasePathList() = 0;
};

struct SQLCipher {
    string password;
    int version;
};

class DatabasePlugin {
    string db_path;
    DatabaseProvider *provider;
    map<string, SQLCipher> cipher;
    shared_ptr<Database> db_con;

    shared_ptr<Database> open();

    vector<string> databasePathList();

    void selectDB(int index);

public:
    DatabasePlugin(HttpServer *server, DatabaseProvider *provider);

    void setCipherKey(string database, string password, int version);
};


#endif //INSPECTOR_DATABASEPLUGIN_H
