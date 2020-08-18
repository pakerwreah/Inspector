//
// Created by Paker on 2019-10-23.
//

#include "Database.h"

Database::Database(const string &path, const string &password, int version) {
    if (!path.length()) {
        throw runtime_error("Database path not selected");
    }
    auto m_path = path.find("file://") == 0 ? path.substr(7) : path;
    auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_WAL | SQLITE_OPEN_SHAREDCACHE;
    auto err = sqlite3_open_v2(m_path.c_str(), &db, flags, nullptr);
    if (err != SQLITE_OK) {
        throw runtime_error("Error opening database (" + to_string(err) + "): " + m_path);
    }

    if (password.length()) {
        execute("PRAGMA cipher_memory_security = OFF");
        execute("PRAGMA key = '" + password + "'");

        if (version) {
            execute("PRAGMA cipher_compatibility = " + to_string(version));
        }
    }

    execute("PRAGMA foreign_keys = ON");
}

Database::~Database() {
    sqlite3_close_v2(db);
}

void Database::commit() const {
    execute("commit transaction");
}

void Database::transaction() const {
    execute("begin exclusive transaction");
}

ResultSet Database::query(const string &sql) const {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK || stmt == nullptr) {
        throw runtime_error(string() + "Error executing query: " + sqlite3_errmsg(db));
    }

    return ResultSet(db, stmt);
}

void Database::execute(const string &sql) const {
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

