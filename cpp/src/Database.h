//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_DATABASE_H
#define INSPECTOR_DATABASE_H

#include "sqlite3.h"
#include <string>
#include <vector>

#include "ResultSet.h"

class Database {
private:
    sqlite3 *db;
    mutable bool failed;

public:
    Database(const std::string &path, const std::string &password = "", int version = 0, bool create = false);

    ~Database();

    void commit() const;

    void transaction() const;

    void rollback() const;

    void execute(const std::string &sql) const;

    ResultSet query(const std::string &sql) const;
};

#endif //INSPECTOR_DATABASE_H
