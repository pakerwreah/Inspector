//
// Created by Paker on 09/11/19.
//

#ifndef INSPECTOR_NETWORKPLUGIN_H
#define INSPECTOR_NETWORKPLUGIN_H

#include "HttpRouter.h"

class NetworkPlugin {
    std::shared_ptr<Client> request_client, response_client;
public:
    NetworkPlugin(HttpRouter *router);

    bool isConnected() const;

    void sendRequest(const std::string &uid, const std::string &headers, const std::string &body);

    void sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool compressed = false);
};


#endif //INSPECTOR_NETWORKPLUGIN_H
