//
// Created by Paker on 2019-10-23.
//

#pragma once

#include "ResultSet.h"

class Database {
private:
    sqlite3 *db;
    mutable bool failed;

public:
    explicit Database(const std::string &path, const std::string &password = "", int version = 0, bool create = false);

    ~Database();

    void commit() const;

    void transaction() const;

    void rollback() const;

    void execute(const std::string &sql) const;

    ResultSet query(const std::string &sql) const;
};
