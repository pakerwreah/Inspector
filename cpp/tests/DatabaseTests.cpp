#include "catch.hpp"
#include "Database.h"
#include <filesystem>

using namespace std;

TEST_CASE("Database - Invalid path") {
    const auto matcher = Catch::Message("Invalid database path");
    REQUIRE_THROWS_MATCHES(Database(""), runtime_error, matcher);
}

TEST_CASE("Database - Does not exist") {
    const string path = "no_database.db";
    const auto matcher = Catch::Message("Error opening database (14): " + path);
    std::filesystem::remove(path);
    REQUIRE_THROWS_MATCHES(Database(path), runtime_error, matcher);
}

TEST_CASE("Database - Create") {
    const string path = "no_database.db";
    std::filesystem::remove(path);
    REQUIRE_NOTHROW(Database(path, "", 0, true));
    REQUIRE(std::filesystem::exists(path));
}

TEST_CASE("Database - SQLCipher") {
    const string path = "sqlchiper1.db";

    SECTION("Create") {
        REQUIRE_NOTHROW([&] {
            std::filesystem::remove(path);
            Database db(path, "123", 4, true);
            db.execute("CREATE TABLE tb_test (id INT, name TEXT)");
        }());
        REQUIRE(std::filesystem::exists(path));
    }

    SECTION("Open") {
        const auto message1 = Catch::Message("Error executing query: file is not a database");
        const auto message2 = Catch::Message("Error executing query: file is encrypted or is not a database");
        const auto matcher = message1 || message2;

        SECTION("No password") {
            REQUIRE_THROWS_MATCHES([&] {
                Database db(path, "", 4);
                db.query("SELECT * FROM tb_test");
            }(), runtime_error, matcher);
        }

        SECTION("Wrong password") {
            REQUIRE_THROWS_MATCHES([&] {
                Database db(path, "111", 4);
                db.query("SELECT * FROM tb_test");
            }(), runtime_error, matcher);
        }

        SECTION("Wrong version") {
            REQUIRE_THROWS_MATCHES([&] {
                Database db(path, "123", 3);
                db.query("SELECT * FROM tb_test");
            }(), runtime_error, matcher);
        }

        SECTION("Success") {
            REQUIRE_NOTHROW([&] {
                Database db(path, "123", 4);
                db.query("SELECT * FROM tb_test");
            }());
        }
    }
}

TEST_CASE("Database - Query") {
    const string path = "database.db";
    std::filesystem::remove(path);
    Database db(path, "", 0, true);
    REQUIRE(std::filesystem::exists(path));

    SECTION("Success") {
        const string create_sql = "CREATE TABLE tb_test (id INT, name TEXT)";
        const string insert_sql = "INSERT INTO tb_test VALUES (1,'teste 1'),(2,'teste 2'),(3,'teste 3')";
        REQUIRE_NOTHROW(db.execute(create_sql + ";" + insert_sql));

        ResultSet r = db.query(
                "SELECT *, id/10.0 as 'decimal', "
                "CASE WHEN id = 2 THEN 'text' ELSE NULL END as 'null' "
                "FROM tb_test");
        vector<string> expected_headers = {"id", "name", "decimal", "null"};
        CHECK(r.headers() == expected_headers);

        using rows_t = vector<tuple<int, string, double>>;
        rows_t expected_rows = {{1, "teste 1", 0.1},
                                {2, "teste 2", 0.2},
                                {3, "teste 3", 0.3}};
        rows_t rows;
        while (r.next()) {
            CHECK(r.type(0) == SQLITE_INTEGER);
            CHECK(r.type(1) == SQLITE_TEXT);
            CHECK(r.type(2) == SQLITE_FLOAT);
            CHECK(r.type(3) == (r.integer(0) == 2 ? SQLITE_TEXT : SQLITE_NULL));
            rows.emplace_back(r.integer(0), r.text(1), r.decimal(2));
        }
        CHECK(rows == expected_rows);
    }

    SECTION("Error") {
        const auto matcher = Catch::Message("Error executing query: no such table: tb_test");
        REQUIRE_THROWS_MATCHES(db.query("SELECT * FROM tb_test"), runtime_error, matcher);
    }

    SECTION("Invalid next") {
        ResultSet r = db.query("CREATE TABLE tb_test (id INT)");
        REQUIRE_NOTHROW(r.next()); // fake first step
        const auto matcher = Catch::Message("Error executing query: table tb_test already exists");
        REQUIRE_THROWS_MATCHES(r.next(), runtime_error, matcher);
    }
}

TEST_CASE("Database - Execute") {
    const string path = "database.db";
    std::filesystem::remove(path);
    Database db(path, "", 0, true);
    REQUIRE(std::filesystem::exists(path));

    SECTION("Success") {
        const string script = "CREATE TABLE tb1 (id INT);CREATE TABLE tb2 (id INT);CREATE TABLE tb3 (id INT)";
        REQUIRE_NOTHROW(db.execute(script));

        ResultSet r = db.query("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table'");
        REQUIRE(r.next());
        CHECK(r.integer(0) == 3);
    }

    SECTION("Error") {
        const string script = "CREATE TABLEs tb1 (id INT)";
        const auto matcher = Catch::Message("Error executing script: near \"TABLEs\": syntax error");
        REQUIRE_THROWS_MATCHES(db.execute(script), runtime_error, matcher);
    }
}

TEST_CASE("Database - Transaction") {
    const string path = "database.db";
    std::filesystem::remove(path);
    Database db(path, "", 0, true);
    REQUIRE(std::filesystem::exists(path));
    REQUIRE_NOTHROW(db.execute("CREATE TABLE tb1 (id INT)"));

    SECTION("Not in transaction") {
        const auto matcher = Catch::Message("Error executing script: cannot commit - no transaction is active");
        CHECK_THROWS_MATCHES(db.commit(), runtime_error, matcher);
    }

    SECTION("Already in transaction") {
        const auto matcher = Catch::Message("Error executing script: cannot start a transaction within a transaction");
        CHECK_NOTHROW(db.transaction());
        CHECK_THROWS_MATCHES(db.transaction(), runtime_error, matcher);
    }

    SECTION("Success") {
        CHECK_NOTHROW(db.transaction());
        CHECK_NOTHROW(db.query("INSERT INTO tb1 VALUES (1)"));
        CHECK_NOTHROW(db.commit());
        CHECK(db.query("SELECT * FROM tb1").next());
    }

    SECTION("Error") {
        db.transaction();
        CHECK_THROWS(db.execute("INSERT INTO tb1 VALUES (1);NOT A VALID SQL"));
        CHECK_NOTHROW(db.commit());
        CHECK_FALSE(db.query("SELECT * FROM tb1").next());
    }

    SECTION("Rollback") {
        db.transaction();
        CHECK_NOTHROW(db.query("INSERT INTO tb1 VALUES (1)"));
        CHECK(db.query("SELECT * FROM tb1").next());
        CHECK_THROWS(db.query("NOT A VALID SQL"));
        CHECK_NOTHROW(db.commit());
        CHECK_FALSE(db.query("SELECT * FROM tb1").next());
    }
}
