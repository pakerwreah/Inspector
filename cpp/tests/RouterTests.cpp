#include "catch.hpp"
#include "Router.h"

using namespace std;

TEST_CASE("Router - Invalid header") {
    Router router;
    Request request;
    CHECK_THROWS_MATCHES(router.handle(request), runtime_error, Catch::Message("Bad Request"));
}

TEST_CASE("Router - No route defined") {
    Router router;
    Request request {.method = "GET", .path = "/test/path"};
    CHECK_THROWS_MATCHES(router.handle(request), out_of_range, Catch::Message("Route not found"));
}

TEST_CASE("Router - Match route") {
    Router router;
    Request request;

    router.get("/test/path/{param}", [](const Request &, const Params &) {
        return Response();
    });

    SECTION("Found") {
        request = {.method = "GET", .path = "/test/path/p_1"};
        REQUIRE_NOTHROW(router.handle(request));
    }

    SECTION("Not found") {
        request = {.method = "GET", .path = "/test/wrong/p_1"};
        CHECK_THROWS_MATCHES(router.handle(request), out_of_range, Catch::Message("Route not found"));
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
        return Response();
    });

    request = {.method = "GET", .path = "/test/path1/p_1/path2/p_2?param3=q%201&param4=q%202"};
    REQUIRE_NOTHROW(router.handle(request));
    CHECK(m_params == expected_params);
}

TEST_CASE("Router - POST") {
    Router router;
    Request request;
    Params m_params;

    router.post("/test/path1/{param1}/path2/{param2}", [&m_params](const Request &, const Params &params) {
        m_params = params;
        return Response();
    });

    SECTION("Should parse body") {
        Params expected_params = {
                {"param1", "p_1"},
                {"param2", "p_2"},
                {"param3", "q 1"},
                {"param4", "q 2"}
        };

        request = {
            .method = "POST",
            .path = "/test/path1/p_1/path2/p_2",
            .body = "param3=q%201&param4=q%202",
        };

        request.headers[Http::ContentType::Key] = Http::ContentType::URL_ENCODED;

        REQUIRE_NOTHROW(router.handle(request));
        CHECK(m_params == expected_params);
    }

    SECTION("Should not parse body") {
        Params expected_params = {
                {"param1", "p_1"},
                {"param2", "p_2"}
        };

        request = {
            .method = "POST",
            .path = "/test/path1/p_1/path2/p_2",
            .body = "param3=q%201&param4=q%202",
        };

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
        request = {.method = "GET", .path = "/test/path"};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 1);
        CHECK(post == 0);
        CHECK(put == 0);
        CHECK(patch == 0);
    }

    SECTION("POST") {
        request = {.method = "POST", .path = "/test/path"};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 0);
        CHECK(post == 1);
        CHECK(put == 0);
        CHECK(patch == 0);
    }

    SECTION("PUT") {
        request = {.method = "PUT", .path = "/test/path"};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 0);
        CHECK(post == 0);
        CHECK(put == 1);
        CHECK(patch == 0);
    }

    SECTION("PATCH") {
        request = {.method = "PATCH", .path = "/test/path"};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(get == 0);
        CHECK(post == 0);
        CHECK(put == 0);
        CHECK(patch == 1);
    }
}
