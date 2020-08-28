#include "catch.hpp"
#include "json.hpp"
#include "Router.h"

using namespace std;
using json = nlohmann::json;

TEST_CASE("Router - Invalid header") {
    Router router;
    Request request;

    router.get("/test/path", [](const Request &, const Params &) {
        return Response();
    });

    string plain_request = "GET /test/path HTTP/1.1\r\n";

    REQUIRE_FALSE(request.parse(plain_request));
    REQUIRE(string(router.handle(request)) == string(Response::BadRequest()));
}

TEST_CASE("Router - Match route") {
    Router router;
    Request request;

    router.get("/test/path/{param}", [](const Request &, const Params &) {
        return Response();
    });

    SECTION("Found") {
        REQUIRE(request.parse("GET /test/path/p_1 HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
    }

    SECTION("Not found") {
        REQUIRE(request.parse("GET /test/wrong/p_1 HTTP/1.1\r\n\r\n"));
        REQUIRE_THROWS_MATCHES(router.handle(request), out_of_range, Catch::Exception::ExceptionMessageMatcher("Route not found"));
    }
}

TEST_CASE("Router - GET") {
    Params expected_params = {
            {"param1", "p_1"},
            {"param2", "p_2"},
            {"param3", "q 1"},
            {"param4", "q 2"}
    };
    Params expected_request_headers = {
            {"Host",       "www.google.com"},
            {"User-Agent", "curl/7.64.1"},
            {"Accept",     "*/*"}
    };
    string expected_response_body = json{{"ok", true}}.dump();
    Params expected_response_headers = {
            {"Content-Type", "application/json"}
    };
    string expected_response =
            "HTTP/1.1 200 \r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 11\r\n\r\n"
            + expected_response_body;

    Router router;
    Request request;
    Response response;
    Params m_params;

    router.get("/test/path1/{param1}/path2/{param2}", [&m_params](const Request &, const Params &params) {
        m_params = params;
        return json{{"ok", true}};
    });

    string plain_request =
            "GET /test/path1/p_1/path2/p_2?param3=q%201&param4=q%202 HTTP/1.1\r\n"
            "Host: www.google.com\r\n"
            "User-Agent: curl/7.64.1\r\n"
            "Accept: */*\r\n\r\n";

    REQUIRE(request.parse(plain_request));

    CHECK(request.method == "GET");
    CHECK(request.path == "/test/path1/p_1/path2/p_2?param3=q%201&param4=q%202");
    CHECK(request.headers == expected_request_headers);
    CHECK(request.body == "");

    REQUIRE_NOTHROW(response = router.handle(request));
    CHECK(m_params == expected_params);

    CHECK(response.code == 200);
    CHECK(response.headers == expected_response_headers);
    CHECK(response.body == expected_response_body);
    CHECK(string(response) == expected_response);
}

TEST_CASE("Router - POST") {
    Params expected_params = {
            {"param1", "p_1"},
            {"param2", "p_2"},
            {"param3", "q 1"},
            {"param4", "q 2"}
    };
    Params expected_request_headers = {
            {"Content-Type", "application/x-www-form-urlencoded"},
            {"Host",         "www.google.com"},
            {"User-Agent",   "curl/7.64.1"},
            {"Accept",       "*/*"}
    };
    string expected_response_body = json{{"ok", true}}.dump();
    Params expected_response_headers = {
            {"Content-Type", "application/json"}
    };
    string expected_response =
            "HTTP/1.1 200 \r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 11\r\n\r\n"
            + expected_response_body;

    Router router;
    Request request;
    Response response;
    Params m_params;

    router.post("/test/path1/{param1}/path2/{param2}", [&m_params](const Request &, const Params &params) {
        m_params = params;
        return json{{"ok", true}};
    });

    string plain_request =
            "POST /test/path1/p_1/path2/p_2 HTTP/1.1\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Host: www.google.com\r\n"
            "User-Agent: curl/7.64.1\r\n"
            "Accept: */*\r\n\r\n"
            "param3=q%201&param4=q%202";

    REQUIRE(request.parse(plain_request));

    CHECK(request.method == "POST");
    CHECK(request.path == "/test/path1/p_1/path2/p_2");
    CHECK(request.headers == expected_request_headers);
    CHECK(request.body == "param3=q%201&param4=q%202");

    REQUIRE_NOTHROW(response = router.handle(request));
    CHECK(m_params == expected_params);

    CHECK(response.code == 200);
    CHECK(response.headers == expected_response_headers);
    CHECK(response.body == expected_response_body);
    CHECK(string(response) == expected_response);
}

TEST_CASE("Router - Methods") {
    Router router;
    Request request;

    int get = 0, post = 0, put = 0, patch = 0;

    router.get("/test/path", [&get](const Request &, const Params &) {
        get++;
        return Response();
    });

    router.post("/test/path", [&post](const Request &, const Params &) {
        post++;
        return Response();
    });

    router.put("/test/path", [&put](const Request &, const Params &) {
        put++;
        return Response();
    });

    router.route("PATCH", "/test/path", [&patch](const Request &, const Params &) {
        patch++;
        return Response();
    });

    SECTION("GET") {
        REQUIRE(request.parse("GET /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        REQUIRE(get == 1);
        REQUIRE(post == 0);
        REQUIRE(put == 0);
        REQUIRE(patch == 0);
    }

    SECTION("POST") {
        REQUIRE(request.parse("POST /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        REQUIRE(get == 0);
        REQUIRE(post == 1);
        REQUIRE(put == 0);
        REQUIRE(patch == 0);
    }

    SECTION("PUT") {
        REQUIRE(request.parse("PUT /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        REQUIRE(get == 0);
        REQUIRE(post == 0);
        REQUIRE(put == 1);
        REQUIRE(patch == 0);
    }

    SECTION("PATCH") {
        REQUIRE(request.parse("PATCH /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        REQUIRE(get == 0);
        REQUIRE(post == 0);
        REQUIRE(put == 0);
        REQUIRE(patch == 1);
    }
}
