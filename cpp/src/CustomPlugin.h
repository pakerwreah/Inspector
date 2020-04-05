//
// Created by Paker on 04/04/20.
//

#ifndef INSPECTOR_CUSTOMPLUGIN_H
#define INSPECTOR_CUSTOMPLUGIN_H

#include <map>
#include <string>
#include <vector>

class HttpServer;

using namespace std;

typedef function<string()> PluginAction;

struct PluginMeta {
    string key, name;
};

class CustomPlugin {
    vector<PluginMeta> plugins;
    map<string, PluginAction> actions;

public:
    CustomPlugin(HttpServer *server);

    void addPlugin(string key, string name, PluginAction action);
};


#endif //INSPECTOR_CUSTOMPLUGIN_H
