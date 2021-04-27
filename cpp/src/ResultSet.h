//
// Created by Paker on 2020-08-23.
//

#pragma once

#include "sqlite3.h"
#include <string>
#include <vector>

class ResultSet {
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int index, first_step;

    bool step();

    ResultSet(sqlite3 *db, sqlite3_stmt *stmt);

    friend class Database;

public:
    virtual ~ResultSet();

    bool next();

    std::vector<std::string> headers() const;

    int type(int column) const;

    std::string text(int column) const;

    int integer(int column) const;

    double decimal(int column) const;
};
