//
// Created by Paker on 04/04/20.
//

#include "CustomPlugin.h"
#include "util.h"

using namespace std;
using nlohmann::json;

void to_json(json &j, const PluginMeta &p) {
    j = {{"key",  p.key},
         {"name", p.name},
         {"live", p.live}};
}

Response CustomPlugin::execute(const PluginAction &action) {
    string res;
    try {
        return json::parse(res = action());
    } catch (const json::parse_error &) {
        return res;
    }
}

CustomPlugin::CustomPlugin(Router &router) : router(router) {
    router.get("/plugins", [this](const Request &, const Params &) {
        return json(plugins);
    });
}

void CustomPlugin::addPlugin(const string &key, const string &name, const PluginAction &action, bool live) {
    plugins.push_back({key, name, live});
    router.get("/plugins/" + key, [action](const Request &, const Params &) {
        return execute([action] {
            return action();
        });
    });
}

void CustomPlugin::addPlugin(const string &key, const string &name, const PluginAction &action) {
    addPlugin(key, name, action, false);
}

void CustomPlugin::addLivePlugin(const string &key, const string &name, const PluginAction &action) {
    addPlugin(key, name, action, true);
}

void CustomPlugin::addPluginAPI(const string &method, const string &path, const PluginAPIAction &action) {
    router.route(
            method,
            "/plugins/api/" + util::ltrim(path, "/"),
            [action](const Request &, const Params &params) {
                return execute([action, &params] {
                    return action(params);
                });
            }
    );
}
