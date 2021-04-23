//
// Created by Paker on 29/10/19.
//

#ifndef INSPECTOR_DATABASEPLUGIN_H
#define INSPECTOR_DATABASEPLUGIN_H

#include "Router.h"
#include "Database.h"
#include <chrono>

class DatabaseProvider {
public:
    virtual std::vector<std::string> databasePathList() const = 0;
};

struct DatabaseMeta {
    std::string password;
    int version;
};

class DatabasePlugin {
private:
    int auto_close_token;
    std::string db_path;
    std::map<std::string, DatabaseMeta> db_meta;
    std::shared_ptr<Database> db_con;
    std::shared_ptr<DatabaseProvider> provider;

protected:
    std::chrono::nanoseconds debounce;

public:
    DatabasePlugin(Router &router, std::shared_ptr<DatabaseProvider> provider);
    virtual ~DatabasePlugin();

    std::vector<std::string> databasePathList();
    void setCipherKey(const std::string &database, const std::string &password, int version);
    std::shared_ptr<Database> open();
    void selectDB(int index);
    std::string databaseName() const;
    bool isOpen() const;
    void setDebounce(std::chrono::nanoseconds debounce);
};

#endif //INSPECTOR_DATABASEPLUGIN_H
