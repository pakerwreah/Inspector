//
// Created by Paker on 29/10/19.
//

#ifndef INSPECTOR_DATABASEPLUGIN_H
#define INSPECTOR_DATABASEPLUGIN_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "Router.h"
#include "Database.h"

class DatabaseProvider {
public:
    virtual std::vector<std::string> databasePathList() = 0;
};

struct DatabaseMeta {
    std::string password;
    int version;
};

class DatabasePlugin {
    int auto_close_token;
    std::string db_path;
    DatabaseProvider *provider;
    std::map<std::string, DatabaseMeta> db_meta;
    std::shared_ptr<Database> db_con;
protected:
    std::chrono::nanoseconds debounce;
public:
    DatabasePlugin(Router *router, DatabaseProvider *provider);
    virtual ~DatabasePlugin();

    std::vector<std::string> databasePathList();
    void setCipherKey(const std::string &database, const std::string &password, int version);
    std::shared_ptr<Database> open();
    void selectDB(int index);
    std::string databaseName() const;
    bool isOpen() const;
};


#endif //INSPECTOR_DATABASEPLUGIN_H
