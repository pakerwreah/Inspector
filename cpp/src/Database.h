//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_DATABASE_H
#define INSPECTOR_DATABASE_H

#include "libs/sqlite3.h"
#include <string>
#include <vector>

using namespace std;

class ResultSet;

class Database {
    sqlite3 *db;
public:
    Database(const string &path, const string &password = "", int version = 0);

    ~Database();

    void commit() const;

    void transaction() const;

    void execute(const string &sql) const;

    ResultSet query(const string &sql) const;
};

class ResultSet {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int index = -1, first_step = -1;

    bool step();

public:
    ResultSet(sqlite3 *db, sqlite3_stmt *stmt);

    virtual ~ResultSet();

    bool next();

    vector<string> headers() const;

    int type(int column) const;

    string text(int column) const;

    int integer(int column) const;

    double decimal(int column) const;
};

#endif //INSPECTOR_DATABASE_H
