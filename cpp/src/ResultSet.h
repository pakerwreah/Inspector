//
// Created by Paker on 2020-08-23.
//

#ifndef INSPECTOR_RESULTSET_H
#define INSPECTOR_RESULTSET_H

#include "sqlite3.h"
#include <string>
#include <vector>

class ResultSet {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int index = -1, first_step = -1;

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

#endif //INSPECTOR_RESULTSET_H
