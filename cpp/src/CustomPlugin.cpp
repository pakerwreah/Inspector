//
// Created by Paker on 04/04/20.
//

#include "CustomPlugin.h"
#include "util.h"

using namespace std;
using json = nlohmann::json;

void to_json(json &j, const PluginMeta &p) {
    j = {{"key",  p.key},
         {"name", p.name},
         {"live", p.live}};
}

Response CustomPlugin::execute(PluginAction action) {
    string res;
    try {
        return Response(json::parse(res = action()));
    } catch (const json::parse_error &) {
        return Response(res, 200, Http::ContentType::HTML);
    }
}

CustomPlugin::CustomPlugin(HttpRouter *router) {
    router->get("/plugins", [this](const Request &, const Params &) {
        return Response(plugins);
    });

    router->get("/plugins/{:key:}", [this](const Request &request, const Params &params) {
        return execute([&] {
            string key = params.at(":key:");
            PluginAction action = actions.at(key);
            return action();
        });
    });

    router->any("/plugins/api/*", [this](const Request &request, const Params &) {
        return execute([&] {
            const auto[action, params] = api_router.parse(request);
            return action(params);
        });
    });
}

void CustomPlugin::addPlugin(const string &key, const string &name, PluginAction action, bool live) {
    plugins.push_back({key, name, live});
    actions[key] = action;
}

void CustomPlugin::addPlugin(const string &key, const string &name, PluginAction action) {
    addPlugin(key, name, action, false);
}

void CustomPlugin::addLivePlugin(const string &key, const string &name, PluginAction action) {
    addPlugin(key, name, action, true);
}

void CustomPlugin::addPluginAPI(const string &method, const string &path, PluginAPIAction action) {
    api_router.route(method, "/plugins/api/" + util::ltrim(path, "/"), action);
}
