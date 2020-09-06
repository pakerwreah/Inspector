#include "catch.hpp"
#include "Router.h"

using namespace std;

namespace Matchers {
    const auto RouteNotFound = Catch::Exception::ExceptionMessageMatcher("Route not found");
}

TEST_CASE("Router - Invalid header") {
    Router router;
    Request request;

    REQUIRE_FALSE(request.parse("GET /test/path HTTP/1.1\r\n"));
    REQUIRE(string(router.handle(request)) == string(Response::BadRequest()));
}

TEST_CASE("Router - No route defined") {
    Router router;
    Request request;

    REQUIRE(request.parse("GET /test/path HTTP/1.1\r\n\r\n"));
    REQUIRE_THROWS_MATCHES(router.handle(request), out_of_range, Matchers::RouteNotFound);
}

TEST_CASE("Router - Match route") {
    Router router;
    Request request;

    router.get("/test/path/{param}", [](const Request &, const Params &) {
        return "";
    });

    SECTION("Found") {
        REQUIRE(request.parse("GET /test/path/p_1 HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
    }

    SECTION("Not found") {
        REQUIRE(request.parse("GET /test/wrong/p_1 HTTP/1.1\r\n\r\n"));
        REQUIRE_THROWS_MATCHES(router.handle(request), out_of_range, Matchers::RouteNotFound);
    }
}

TEST_CASE("Router - GET") {
    Params expected_params = {
            {"param1", "p_1"},
            {"param2", "p_2"},
            {"param3", "q 1"},
            {"param4", "q 2"}
    };

    Router router;
    Request request;
    Params m_params;

    router.get("/test/path1/{param1}/path2/{param2}", [&m_params](const Request &, const Params &params) {
        m_params = params;
        return "";
    });

    REQUIRE(request.parse("GET /test/path1/p_1/path2/p_2?param3=q%201&param4=q%202 HTTP/1.1\r\n\r\n"));
    REQUIRE_NOTHROW(router.handle(request));
    CHECK(m_params == expected_params);
}

TEST_CASE("Router - POST") {
    Router router;
    Request request;
    Params m_params;

    router.post("/test/path1/{param1}/path2/{param2}", [&m_params](const Request &, const Params &params) {
        m_params = params;
        return "";
    });

    SECTION("Should parse body") {
        Params expected_params = {
                {"param1", "p_1"},
                {"param2", "p_2"},
                {"param3", "q 1"},
                {"param4", "q 2"}
        };
        string plain_request =
                "POST /test/path1/p_1/path2/p_2 HTTP/1.1\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "\r\n"
                "param3=q%201&param4=q%202";

        REQUIRE(request.parse(plain_request));
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(m_params == expected_params);
    }

    SECTION("Should not parse body") {
        Params expected_params = {
                {"param1", "p_1"},
                {"param2", "p_2"}
        };
        string plain_request =
                "POST /test/path1/p_1/path2/p_2 HTTP/1.1\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "param3=q%201&param4=q%202";

        REQUIRE(request.parse(plain_request));
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(m_params == expected_params);
    }
}

TEST_CASE("Router - Methods") {
    Router router;
    Request request;

    int get = 0, post = 0, put = 0, patch = 0;

    router.get("/test/path", [&get](const Request &, const Params &) {
        get++;
        return "";
    });

    router.post("/test/path", [&post](const Request &, const Params &) {
        post++;
        return "";
    });

    router.put("/test/path", [&put](const Request &, const Params &) {
        put++;
        return "";
    });

    router.route("PATCH", "/test/path", [&patch](const Request &, const Params &) {
        patch++;
        return "";
    });

    SECTION("GET") {
        REQUIRE(request.parse("GET /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 1);
        CHECK(post == 0);
        CHECK(put == 0);
        CHECK(patch == 0);
    }

    SECTION("POST") {
        REQUIRE(request.parse("POST /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 0);
        CHECK(post == 1);
        CHECK(put == 0);
        CHECK(patch == 0);
    }

    SECTION("PUT") {
        REQUIRE(request.parse("PUT /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 0);
        CHECK(post == 0);
        CHECK(put == 1);
        CHECK(patch == 0);
    }

    SECTION("PATCH") {
        REQUIRE(request.parse("PATCH /test/path HTTP/1.1\r\n\r\n"));
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 0);
        CHECK(post == 0);
        CHECK(put == 0);
        CHECK(patch == 1);
    }
}
