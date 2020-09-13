//
// Created by Paker on 2020-08-23.
//

#include "ResultSet.h"
#include <stdexcept>

using namespace std;

ResultSet::ResultSet(sqlite3 *db, sqlite3_stmt *stmt) {
    this->db = db;
    this->stmt = stmt;
    sqlite3_reset(stmt);
    first_step = step();
}

ResultSet::~ResultSet() {
    sqlite3_finalize(stmt);
}

vector<string> ResultSet::headers() const {
    vector<string> headers;
    int count = sqlite3_column_count(stmt);
    for (int i = 0; i < count; i++) {
        headers.push_back(sqlite3_column_name(stmt, i));
    }
    return headers;
}

bool ResultSet::step() {
    index++;
    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
        const string errmsg = sqlite3_errmsg(db);
        throw runtime_error("Error executing query: " + errmsg);
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

int ResultSet::type(int column) const {
    return sqlite3_column_type(stmt, column);
}

string ResultSet::text(int column) const {
    return (const char *) sqlite3_column_text(stmt, column);
}

int ResultSet::integer(int column) const {
    return sqlite3_column_int(stmt, column);
}

double ResultSet::decimal(int column) const {
    return sqlite3_column_double(stmt, column);
}

