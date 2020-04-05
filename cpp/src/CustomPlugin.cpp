//
// Created by Paker on 04/04/20.
//

#include "CustomPlugin.h"
#include "HttpServer.h"

void to_json(json &j, const PluginMeta &p) {
    j = {{"key",  p.key},
         {"name", p.name}};
}

void CustomPlugin::addPlugin(string key, string name, PluginAction action) {
    plugins.push_back({key, name});
    actions[key] = action;
}

CustomPlugin::CustomPlugin(HttpServer *server) {
    server->get("/plugins", [this](Request req) {
        return Response(plugins);
    });

    server->get("/plugins/{key}", [this](Request req) {
        try {
            auto action = actions.at(req.params["key"]);
            auto res = action();
            try {
                return Response(json::parse(res));
            } catch (exception &ex) {
                return Response(res, 200, ContentType::HTML);
            }
        } catch (out_of_range &ex) {
            return Response(ex.what(), 400);
        } catch (exception &ex) {
            return Response(ex.what(), 500);
        }
    });
}