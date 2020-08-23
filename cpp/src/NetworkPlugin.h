//
// Created by Paker on 09/11/19.
//

#ifndef INSPECTOR_NETWORKPLUGIN_H
#define INSPECTOR_NETWORKPLUGIN_H

#include "HttpServer.h"

class NetworkPlugin {
    std::shared_ptr<Socket> request_socket, response_socket;
public:
    NetworkPlugin(HttpServer *server);

    bool isConnected() const;

    void sendRequest(const std::string &uid, const std::string &headers, const std::string &body);

    void sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool compressed = false);
};


#endif //INSPECTOR_NETWORKPLUGIN_H
