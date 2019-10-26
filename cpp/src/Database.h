//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_DATABASE_H
#define INSPECTOR_DATABASE_H

#include "sqlite3.h"
#include <string>
#include <vector>

using namespace std;

class ResultSet;

class Database {
    sqlite3 *db;
public:
    Database(string path);

    virtual ~Database();

    void commit();

    void transaction();

    void execute(string sql);

    ResultSet query(string sql);
};

class ResultSet {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int index = -1, first_step = -1;
    vector<string> _headers;

    bool step();

public:
    ResultSet(sqlite3 *db, sqlite3_stmt *stmt);

    virtual ~ResultSet();

    bool next();

    vector<string> headers();

    int type(int column);

    string text(int column);

    int integer(int column);

    double decimal(int column);
};

#endif //INSPECTOR_DATABASE_H
