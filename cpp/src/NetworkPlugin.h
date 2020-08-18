//
// Created by Paker on 09/11/19.
//

#ifndef INSPECTOR_NETWORKPLUGIN_H
#define INSPECTOR_NETWORKPLUGIN_H

#include <iostream>

class HttpServer;

class Socket;

using namespace std;

class NetworkPlugin {
    shared_ptr<Socket> request_socket, response_socket;
public:
    NetworkPlugin(HttpServer *server);

    bool isConnected() const;

    void sendRequest(const string &uid, const string &headers, const string &body);

    void sendResponse(const string &uid, const string &headers, const string &body, bool compressed = false);
};


#endif //INSPECTOR_NETWORKPLUGIN_H
