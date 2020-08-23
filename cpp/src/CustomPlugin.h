//
// Created by Paker on 04/04/20.
//

#ifndef INSPECTOR_CUSTOMPLUGIN_H
#define INSPECTOR_CUSTOMPLUGIN_H

#include <map>
#include <string>
#include <vector>
#include <functional>

#include "HttpServer.h"

typedef std::function<std::string()> PluginAction;
typedef std::function<std::string(const Params &)> PluginAPIAction;

struct PluginMeta {
    std::string key, name;
    bool live;
};

class CustomPlugin {
    std::vector<PluginMeta> plugins;

    // plugin-key: action
    std::map<std::string, PluginAction> actions;

    // method: { path: action }
    std::map<std::string, std::map<std::string, PluginAPIAction>> api;

    Response execute(PluginAction executor) noexcept;

    void addPlugin(const std::string &key, const std::string &name, PluginAction action, bool live);

public:
    CustomPlugin(HttpServer *server);

    void addPlugin(const std::string &key, const std::string &name, PluginAction action);

    void addPluginAPI(const std::string &method, const std::string &path, PluginAPIAction action);

    void addLivePlugin(const std::string &key, const std::string &name, PluginAction action);
};


#endif //INSPECTOR_CUSTOMPLUGIN_H
