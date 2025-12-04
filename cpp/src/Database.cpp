//
// Created by Paker on 2019-10-23.
//

#include "Database.h"
#include <stdexcept>

Database::Database(const std::string &path, const std::string &password, int version, bool create) : db(nullptr), failed(false) {

    if (!path.length()) {
        throw std::runtime_error("Invalid database path");
    }
    auto m_path = path.find("file://") == 0 ? path.substr(7) : path;
    auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_WAL | SQLITE_OPEN_SHAREDCACHE;
    if (create) {
        flags |= SQLITE_OPEN_CREATE;
    }
    auto err = sqlite3_open_v2(m_path.c_str(), &db, flags, nullptr);
    if (err != SQLITE_OK) {
        throw std::runtime_error("Error opening database (" + std::to_string(err) + "): " + m_path);
    }

    if (password.length()) {
        execute("PRAGMA cipher_memory_security = OFF");
        execute("PRAGMA key = '" + password + "'");

        if (version) {
            execute("PRAGMA cipher_compatibility = " + std::to_string(version));
        }
    }

    execute("PRAGMA foreign_keys = ON");
}

Database::~Database() {
    sqlite3_close_v2(db);
}

void Database::commit() const {
    if (failed) {
        rollback();
    } else {
        execute("commit transaction");
    }
}

void Database::transaction() const {
    failed = false;
    execute("begin exclusive transaction");
}

void Database::rollback() const {
    sqlite3_exec(db, "rollback", nullptr, nullptr, nullptr);
}

ResultSet Database::query(const std::string &sql) const {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK || stmt == nullptr) {
        failed = true;
        const std::string errmsg = sqlite3_errmsg(db);
        throw std::runtime_error("Error executing query: " + errmsg);
    }

    return ResultSet(db, stmt);
}

void Database::execute(const std::string &sql) const {
    char *error;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error);

    if (error) {
        failed = true;
        const std::string errmsg = error;
        sqlite3_free(error);
        throw std::runtime_error("Error executing script: " + errmsg);
    }
}
