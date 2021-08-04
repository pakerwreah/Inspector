//
// Created by Paker on 04/04/20.
//

#pragma once

#include "Router.h"

typedef std::function<std::string()> PluginAction;
typedef std::function<std::string(const Params &)> PluginAPIAction;

struct PluginMeta {
    std::string key, name;
    bool live;
};

void to_json(nlohmann::json &j, const PluginMeta &p);

class CustomPlugin {
private:
    Router &router;
    std::vector<PluginMeta> plugins;
    std::map<std::string, PluginAction> actions;

    void addPlugin(const std::string &key, const std::string &name, const PluginAction &action, bool live);

public:
    explicit CustomPlugin(Router &router);

    static Response execute(const PluginAction &action);

    void addPlugin(const std::string &key, const std::string &name, const PluginAction &action);
    void addPluginAPI(const std::string &method, const std::string &path, const PluginAPIAction &action);
    void addLivePlugin(const std::string &key, const std::string &name, const PluginAction &action);
};
