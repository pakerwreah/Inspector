//
// Created by Paker on 2019-10-23.
//

#include <cassert>
#include "Database.h"

Database::Database(string path) {
    if (!path.length()) {
        throw runtime_error("Database path not selected");
    }
    if (path.find("file://") == 0) {
        path = path.substr(7);
    }
    auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    auto err = sqlite3_open_v2(path.c_str(), &db, flags, nullptr);
    if (err != SQLITE_OK) {
        throw runtime_error("Error opening database (" + to_string(err) + "): " + path);
    }

    execute("PRAGMA foreign_keys = ON");
    execute("PRAGMA journal_mode = WAL");
}

Database::~Database() {
    sqlite3_close_v2(db);
}

void Database::commit() {
    execute("commit transaction");
}

void Database::transaction() {
    execute("begin exclusive transaction");
}

ResultSet Database::query(string sql) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw runtime_error(string() + "Error executing query: " + sqlite3_errmsg(db));
    }

    return ResultSet(db, stmt);
}

void Database::execute(string sql) {
    char *error;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error);

    if (error) {
        string str = error;
        sqlite3_free(error);
        throw runtime_error("Error executing batch: " + str);
    }
}

ResultSet::ResultSet(sqlite3 *db, sqlite3_stmt *stmt) {
    this->db = db;
    this->stmt = stmt;
    sqlite3_reset(stmt);
    first_step = step();
}

ResultSet::~ResultSet() {
    sqlite3_finalize(stmt);
}

vector<string> ResultSet::headers() {
    if (!_headers.size()) {
        int count = sqlite3_column_count(stmt);
        for (int i = 0; i < count; i++) {
            _headers.push_back(sqlite3_column_name(stmt, i));
        }
    }
    return _headers;
}

bool ResultSet::step() {
    index++;
    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
        throw runtime_error(string() + sqlite3_errmsg(db) + "\nSQL: " + sqlite3_sql(stmt));
    }

    return rc == SQLITE_ROW;
}

bool ResultSet::next() {
    if (first_step >= 0) {
        auto res = first_step;
        first_step = -1;
        return res;
    }

    return step();
}

int ResultSet::type(int column) {
    return sqlite3_column_type(stmt, column);
}

string ResultSet::text(int column) {
    return (const char *) sqlite3_column_text(stmt, column);
}

int ResultSet::integer(int column) {
    return sqlite3_column_int(stmt, column);
}

double ResultSet::decimal(int column) {
    return sqlite3_column_double(stmt, column);
}

