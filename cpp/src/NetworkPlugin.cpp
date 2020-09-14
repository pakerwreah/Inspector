//
// Created by Paker on 09/11/19.
//

#include "NetworkPlugin.h"
#include "compress.hpp"
#include <thread>

using namespace std;

NetworkPlugin::NetworkPlugin(Router *router) {
    router->get("/network/request", [this](const Request &request, const Params &) {
        request_client = make_shared<WebSocket>(request.client);
        return WebSocket::handshake(request);
    });

    router->get("/network/response", [this](const Request &request, const Params &) {
        response_client = make_shared<WebSocket>(request.client);
        return WebSocket::handshake(request);
    });
}

bool NetworkPlugin::isConnected() const {
    return request_client && response_client;
}

void NetworkPlugin::sendRequest(const string &uid, const string &headers, const string &body) {
    // create local reference to avoid deallocation
    auto client = request_client;
    if (client) {
        // create a thread to avoid hanging the client
        thread([=] {
            const string m_body = gzip::compress(body.c_str(), body.size());
            if (!client->send(uid + "\n" + headers + "\n" + m_body, true)) {
                request_client = nullptr;
            }
        }).detach();
    }
}

void NetworkPlugin::sendResponse(const string &uid, const string &headers, const string &body, bool compressed) {
    // create local reference to avoid deallocation
    auto client = response_client;
    if (client) {
        // create a thread to avoid hanging the client
        thread([=] {
            string m_body = body;
            if (!compressed) {
                m_body = gzip::compress(body.c_str(), body.size());
            }
            if (!client->send(uid + "\n" + headers + "\n" + m_body, true)) {
                response_client = nullptr;
            }
        }).detach();
    }
}
