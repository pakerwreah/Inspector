//
// Created by Paker on 09/11/19.
//

#ifndef INSPECTOR_NETWORKPLUGIN_H
#define INSPECTOR_NETWORKPLUGIN_H

#include "Router.h"
#include "WebSocket.h"
#include <shared_mutex>
#include <set>

class NetworkPlugin {
    std::set<std::shared_ptr<WebSocket>> request_clients, response_clients;
    std::shared_mutex mutex;
public:
    NetworkPlugin(Router *router);

    bool isConnected() const;

    void sendRequest(const std::string &uid, const std::string &headers, const std::string &body);

    void sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool compressed = false);
};


#endif //INSPECTOR_NETWORKPLUGIN_H
