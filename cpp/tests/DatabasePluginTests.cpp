#include "catch.hpp"
#include "DatabasePlugin.h"
#include "MockDatabaseProvider.h"
#include "MockDatabasePlugin.h"
#include <filesystem>
#include <thread>

using namespace std;
using json = nlohmann::json;

TEST_CASE("DatabasePlugin - Provider") {
    Router router;
    MockDatabaseProvider databaseProvider;
    DatabasePlugin plugin(&router, &databaseProvider);
    databaseProvider.databases = {"database1.db", "database2.db"};

    CHECK(plugin.databasePathList() == databaseProvider.databasePathList());
}

TEST_CASE("DatabasePlugin - Open") {
    Router router;
    MockDatabaseProvider databaseProvider;
    MockDatabasePlugin plugin(&router, &databaseProvider);

    REQUIRE_NOTHROW([] {
        const string path = "database1.db";
        filesystem::remove(path);
        Database(path, "", 0, true);
    }());

    SECTION("No database") {
        CHECK_THROWS_MATCHES(plugin.open(), runtime_error, Catch::Message("No databases available"));
        CHECK_FALSE(plugin.isOpen());
    }

    SECTION("Database does not exist") {
        const string path = "no_database.db";
        filesystem::remove(path);
        databaseProvider.databases = {path};
        const auto matcher = Catch::Message("Error opening database (14): " + path);
        CHECK_THROWS_MATCHES(plugin.open(), runtime_error, matcher);
    }

    SECTION("Success") {
        databaseProvider.databases = {"database1.db"};
        REQUIRE_NOTHROW(plugin.open());
        CHECK(plugin.isOpen());
        CHECK(plugin.databaseName() == "database1.db");
    }

    SECTION("Debounce") {
        plugin.setDebounce(50ms);
        databaseProvider.databases = {"database1.db"};
        REQUIRE_NOTHROW(plugin.open());
        CHECK(plugin.isOpen());

        for (int i = 0; i < 3; i++) {
            this_thread::sleep_for(20ms);
            REQUIRE_NOTHROW(plugin.open());
            CHECK(plugin.isOpen());
        }

        this_thread::sleep_for(100ms);
        CHECK_FALSE(plugin.isOpen());
    }
}

TEST_CASE("DatabasePlugin - Select database") {
    Router router;
    MockDatabaseProvider databaseProvider;
    DatabasePlugin plugin(&router, &databaseProvider);

    SECTION("In Code") {
        databaseProvider.databases = {"path/database1.db", "path/database2.db"};

        CHECK_NOTHROW(plugin.selectDB(0));
        CHECK_FALSE(plugin.isOpen());
        CHECK(plugin.databaseName() == "database1.db");

        CHECK_THROWS_MATCHES(plugin.selectDB(2), out_of_range, Catch::Message("Index out of range"));
        CHECK_FALSE(plugin.isOpen());
        CHECK(plugin.databaseName() == "database1.db");
    }

    SECTION("Route") {
        Request request;
        Response response;

        databaseProvider.databases = {"database1.db", "database2.db"};

        REQUIRE_NOTHROW([] {
            const string path = "database2.db";
            filesystem::remove(path);
            Database(path, "", 0, true);
        }());

        request = {"PUT", "/database/current/1"};

        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(response.code == 200);
        CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::HTML);
        CHECK(response.body == "database2.db");
    }
}

TEST_CASE("DatabasePlugin - List") {
    Router router;
    Request request;
    Response response;
    MockDatabaseProvider databaseProvider;
    DatabasePlugin plugin(&router, &databaseProvider);
    databaseProvider.databases = {"file://path/database1.db", "file://path/database2.db"};
    CHECK_NOTHROW(plugin.selectDB(1));

    json expected = {{"databases", {"database1.db", "database2.db"}},
                     {"current",   1}};

    request = {"GET", "/database/list"};

    REQUIRE_NOTHROW(response = router.handle(request));
    CHECK(response.code == 200);
    CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::JSON);
    CHECK(response.body == expected.dump());
}

TEST_CASE("DatabasePlugin - Encryption") {
    Router router;
    Request request;
    Response response;
    MockDatabaseProvider databaseProvider;
    DatabasePlugin plugin(&router, &databaseProvider);
    const string path = "database1.db";
    databaseProvider.databases = {path};

    REQUIRE_NOTHROW([&] {
        filesystem::remove(path);
        Database db(path, "123", 4, true);
        db.execute("CREATE TABLE tb_test (id INT, name TEXT)");
    }());

    request = {"POST", "/database/query", "SELECT * FROM tb_test"};

    SECTION("Fail") {
        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(string(response) == string(Response::BadRequest("Error executing query: file is not a database")));
    }

    SECTION("Success") {
        CHECK_NOTHROW(plugin.setCipherKey("database1.db", "123", 4));

        string expected = json{{"databases", {"database1.db", "database2.db"}},
                               {"current",   1}}.dump();

        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(response.code == 200);
        CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::JSON);
    }
}


TEST_CASE("DatabasePlugin - Query") {
    Router router;
    Request request;
    Response response;
    MockDatabaseProvider databaseProvider;
    DatabasePlugin plugin(&router, &databaseProvider);
    const string path = "database1.db";
    databaseProvider.databases = {path};

    REQUIRE_NOTHROW([&] {
        filesystem::remove(path);
        Database db(path, "", 0, true);
        db.execute("CREATE TABLE tb_test (id INT, name TEXT);"
                   "INSERT INTO tb_test VALUES (1, 'tuple 1'), (2, 'tuple 2')");
    }());

    request = {"POST",
               "/database/query",
               "SELECT *, id/10.0 as 'decimal', "
               "CASE WHEN id = 2 THEN 'text' ELSE NULL END as 'null' "
               "FROM tb_test"};

    REQUIRE_NOTHROW(response = router.handle(request));
    CHECK(response.code == 200);
    CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::JSON);

    const json &data = json::parse(response.body);
    time_t usec = 0;
    CHECK_NOTHROW(usec = data.at("duration").at("usec"));
    json::array_t rows = {{1, "tuple 1", 0.1, "NULL"},
                          {2, "tuple 2", 0.2, "text"}};
    json duration = {{"sec",  0},
                     {"usec", usec}};
    json::array_t headers = {"id", "name", "decimal", "null"};
    json expected = json{{"data",     rows},
                         {"duration", duration},
                         {"headers",  headers}};
    CHECK(response.body == expected.dump());
}

TEST_CASE("DatabasePlugin - Execute") {
    Router router;
    Request request;
    Response response;
    MockDatabaseProvider databaseProvider;
    DatabasePlugin plugin(&router, &databaseProvider);

    const string path = "database1.db";
    databaseProvider.databases = {path};
    filesystem::remove(path);
    Database db(path, "", 0, true);

    request = {"POST",
               "/database/execute",
               "CREATE TABLE tb_test (id INT, name TEXT);"
               "INSERT INTO tb_test VALUES (1, 'tuple 1');"
               "INSERT INTO tb_test VALUES (2, 'tuple 2')"};

    SECTION("Success") {
        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(response.code == 200);
        CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::JSON);

        const json &data = json::parse(response.body);
        time_t usec = 0;
        CHECK_NOTHROW(usec = data.at("duration").at("usec"));
        json duration = {{"sec",  0},
                         {"usec", usec}};
        json expected = json{{"duration", duration}};
        CHECK(response.body == expected.dump());

        ResultSet r = db.query("SELECT * FROM tb_test");
        using rows_t = vector<tuple<int, string>>;
        rows_t expected_rows = {{1, "tuple 1"},
                                {2, "tuple 2"}};
        rows_t rows;
        while (r.next()) {
            rows.emplace_back(r.integer(0), r.text(1));
        }
        CHECK(rows == expected_rows);
    }

    SECTION("Rollback") {
        request.body += "NOT A VALID SQL";
        REQUIRE_NOTHROW(response = router.handle(request));
        CHECK(string(response) == string(Response::BadRequest("Error executing script: near \"NOT\": syntax error")));

        const auto matcher = Catch::Message("Error executing query: no such table: tb_test");
        CHECK_THROWS_MATCHES(db.query("SELECT * FROM tb_test"), runtime_error, matcher);
    }
}
