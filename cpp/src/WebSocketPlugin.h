//
// Created by Paker on 25/09/20.
//

#ifndef INSPECTOR_WEBSOCKETPLUGIN_H
#define INSPECTOR_WEBSOCKETPLUGIN_H

#include "Router.h"
#include "WebSocket.h"
#include <mutex>
#include <map>

class WebSocketPlugin {
private:
    std::multimap<std::string, std::shared_ptr<WebSocket>> clients;
    std::mutex mutex;

public:
    WebSocketPlugin(Router &router);

    bool isConnected() const;

    void sendMessage(const std::string &key, const std::string &message);
};

#endif //INSPECTOR_WEBSOCKETPLUGIN_H
