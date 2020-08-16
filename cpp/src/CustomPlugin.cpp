//
// Created by Paker on 04/04/20.
//

#include "CustomPlugin.h"
#include "HttpServer.h"
#include "util.h"

void to_json(json &j, const PluginMeta &p) {
    j = {{"key",  p.key},
         {"name", p.name}};
}

void CustomPlugin::addPlugin(const string &key, const string &name, const PluginAction &action) {
    plugins.push_back({key, name});
    actions[key] = action;
}

void CustomPlugin::addPluginAPI(const string &method, const string &path, const PluginAPIAction &action) {
    api[method][util::replaceAll(path, "/", "-")] = action;
}

Response CustomPlugin::execute(PluginAction action) noexcept {
    string res;
    try {
        return Response(json::parse(res = action()));
    } catch (json::parse_error &ex) {
        return Response(res, 200, ContentType::HTML);
    } catch (out_of_range &ex) {
        return Response(ex.what(), 400);
    } catch (exception &ex) {
        return Response(ex.what(), 500);
    }
}

CustomPlugin::CustomPlugin(HttpServer *server) {
    server->get("/plugins", [this](const Request &, const Params &) {
        return Response(plugins);
    });

    server->get("/plugins/{:key:}", [this](const Request &, const Params &params) {
        return execute([&] {
            string key = params.at(":key:");
            PluginAction action = actions.at(key);
            return action();
        });
    });

    server->request("/plugins/api/{:path:}", [this](const Request &request, const Params &params) {
        return execute([&] {
            string path = params.at(":path:");
            PluginAPIAction action = api.at(request.method).at(path);
            return action(params);
        });
    });
}