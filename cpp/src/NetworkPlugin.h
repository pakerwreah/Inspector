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

    void sendRequest(string uid, string headers, string body);

    void sendResponse(string uid, string headers, string body);
};


#endif //INSPECTOR_NETWORKPLUGIN_H
