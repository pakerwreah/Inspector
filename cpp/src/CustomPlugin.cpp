//
// Created by Paker on 04/04/20.
//

#include "CustomPlugin.h"
#include "HttpServer.h"
#include "util.h"
#include <fstream>

void to_json(json &j, const PluginMeta &p) {
    j = {{"key",  p.key},
         {"name", p.name}};
}

void CustomPlugin::addPlugin(const string &key, const string &name, PluginAction action) {
    plugins.push_back({key, name});
    actions[key] = action;
}

void CustomPlugin::addLivePlugin(const string &key, const string &name, const string &filepath) {
    addLivePlugin(key, name, [&filepath] {
        ifstream file(filepath);
        ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    });
}

void CustomPlugin::addLivePlugin(const string &key, const string &name, PluginAction action) {
    addPlugin(key, name, [&] {
        string src = "http://" + host + "/plugins/api/" + key + "/frontend?" + util::uid();
        ostringstream os;
        os << "<div class='absolute-expand d-flex'>";
        os << "     <iframe class='flex' style='border:none' src='" + src + "'></iframe>";
        os << "</div>";
        return os.str();
    });
    addPluginAPI("GET", key + "/frontend", [action](const Params &) {
        return action();
    });
}

void CustomPlugin::addPluginAPI(const string &method, const string &path, PluginAPIAction action) {
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

    server->get("/plugins/{:key:}", [this](const Request &request, const Params &params) {
        host = request.headers.at("Host");
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