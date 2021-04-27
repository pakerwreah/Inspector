#include "catch.hpp"
#include "CustomPlugin.h"

using namespace std;
using nlohmann::json;

TEST_CASE("CustomPlugin - Execute") {
    Router router;
    Response response;
    CustomPlugin plugin(router);

    SECTION("HTML") {
        string content = "text";
        CHECK_NOTHROW(response = plugin.execute([&] {
            return content;
        }));
        CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::HTML);
        CHECK(response.code == 200);
        CHECK(response.body == content);
    }

    SECTION("JSON") {
        string content = "{\"json\":true}";
        CHECK_NOTHROW(response = plugin.execute([&] {
            return content;
        }));
        CHECK(response.headers[Http::ContentType::Key] == Http::ContentType::JSON);
        CHECK(response.code == 200);
        CHECK(response.body == content);
    }

    SECTION("Fail") {
        struct MyException : public exception {
        };
        CHECK_THROWS_AS(plugin.execute([] {
            return throw MyException(), "";
        }), MyException);
    }
}

TEST_CASE("CustomPlugin - PluginMeta") {
    PluginMeta meta{"key-val", "name-val", true};
    json expected = {{"key",  "key-val"},
                     {"name", "name-val"},
                     {"live", true}};
    CHECK(meta == expected);
}

TEST_CASE("CustomPlugin - Routes") {
    Router router;
    Request request;
    Response response;
    CustomPlugin plugin(router);

    int normal = 0, live = 0, api_get = 0, api_post = 0;

    Params api_params;

    plugin.addPlugin("normal", "Normal Plugin", [&normal] {
        return normal++, "";
    });

    plugin.addLivePlugin("live", "Live Plugin", [&live] {
        return live++, "";
    });

    plugin.addPluginAPI("GET", "path/test", [&api_get, &api_params](const Params &params) {
        return api_get++, api_params = params, "";
    });

    plugin.addPluginAPI("POST", "path/test", [&api_post, &api_params](const Params &params) {
        return api_post++, api_params = params, "";
    });

    SECTION("Get Plugins") {
        request = {"GET", "/plugins"};
        REQUIRE_NOTHROW(response = router.handle(request));
        PluginMeta expected[] = {
                {"normal", "Normal Plugin", false},
                {"live",   "Live Plugin",   true}
        };
        CHECK(response.body == json(expected).dump());
    }

    SECTION("Normal Plugin") {
        request = {"GET", "/plugins/normal"};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(normal == 1);
        CHECK(live == 0);
        CHECK(api_get == 0);
        CHECK(api_post == 0);
    }

    SECTION("Live Plugin") {
        request = {"GET", "/plugins/live"};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(normal == 0);
        CHECK(live == 1);
        CHECK(api_get == 0);
        CHECK(api_post == 0);
    }

    SECTION("API GET") {
        request = {"GET", "/plugins/api/path/test?p=123&q=234"};
        Params expected{{"p", "123"},
                        {"q", "234"}};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(normal == 0);
        CHECK(live == 0);
        CHECK(api_get == 1);
        CHECK(api_post == 0);
        CHECK(api_params == expected);
    }

    SECTION("API POST") {
        request = {"POST", "/plugins/api/path/test"};
        request.headers[Http::ContentType::Key] = Http::ContentType::URL_ENCODED;
        request.body = "p=234&q=345";
        Params expected{{"p", "234"},
                        {"q", "345"}};
        REQUIRE_NOTHROW(router.handle(request));
        CHECK(normal == 0);
        CHECK(live == 0);
        CHECK(api_get == 0);
        CHECK(api_post == 1);
        CHECK(api_params == expected);
    }
}
