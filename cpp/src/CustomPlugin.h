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
    bool live;
};

class CustomPlugin {
    vector<PluginMeta> plugins;
    map<PluginKey, PluginAction> actions;
    map<Method, map<Path, PluginAPIAction>> api;

    Response execute(PluginAction executor) noexcept;

    void addPlugin(const string &key, const string &name, PluginAction action, bool live);

public:
    CustomPlugin(HttpServer *server);

    void addPlugin(const string &key, const string &name, PluginAction action);

    void addPluginAPI(const string &method, const string &path, PluginAPIAction action);

    void addLivePlugin(const string &key, const string &name, PluginAction action);
};


#endif //INSPECTOR_CUSTOMPLUGIN_H
