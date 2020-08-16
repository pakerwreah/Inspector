//
// Created by Paker on 04/04/20.
//

#ifndef INSPECTOR_CUSTOMPLUGIN_H
#define INSPECTOR_CUSTOMPLUGIN_H

#include <map>
#include <string>
#include <vector>
#include "HttpServer.h"

using namespace std;

typedef string PluginKey;
typedef function<string()> PluginAction;
typedef function<string(const Params &)> PluginAPIAction;

struct PluginMeta {
    string key, name;
};

class CustomPlugin {
    vector<PluginMeta> plugins;
    map<PluginKey, PluginAction> actions;
    map<Method, map<Path, PluginAPIAction>> api;

    Response execute(PluginAction executor) noexcept;

public:
    CustomPlugin(HttpServer *server);

    void addPlugin(const string &key, const string &name, const PluginAction &action);
    void addPluginAPI(const string &method, const string &path, const PluginAPIAction &action);
};


#endif //INSPECTOR_CUSTOMPLUGIN_H
