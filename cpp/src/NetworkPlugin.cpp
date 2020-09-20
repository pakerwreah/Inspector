//
// Created by Paker on 09/11/19.
//

#include "NetworkPlugin.h"
#include "compress.hpp"
#include <thread>

using namespace std;

NetworkPlugin::NetworkPlugin(Router *router) {
    router->get("/network/request", [this](const Request &request, const Params &) {
        lock_guard guard(mutex);
        request_clients.insert(make_shared<WebSocket>(request.client));
        return WebSocket::handshake(request);
    });

    router->get("/network/response", [this](const Request &request, const Params &) {
        lock_guard guard(mutex);
        response_clients.insert(make_shared<WebSocket>(request.client));
        return WebSocket::handshake(request);
    });
}

bool NetworkPlugin::isConnected() const {
    return request_clients.size() && response_clients.size();
}

void NetworkPlugin::sendRequest(const string &uid, const string &headers, const string &body) {
    shared_lock guard(mutex);
    for (shared_ptr client : request_clients) {
        // create a thread to avoid hanging the client
        thread([=] {
            const string m_body = gzip::compress(body.c_str(), body.size());
            if (!client->send(uid + "\n" + headers + "\n" + m_body, true)) {
                lock_guard guard(mutex);
                request_clients.erase(client);
            }
        }).detach();
    }
}

void NetworkPlugin::sendResponse(const string &uid, const string &headers, const string &body, bool compressed) {
    shared_lock guard(mutex);
    for (shared_ptr client : response_clients) {
        // create a thread to avoid hanging the client
        thread([=] {
            string m_body = body;
            if (!compressed) {
                m_body = gzip::compress(body.c_str(), body.size());
            }
            if (!client->send(uid + "\n" + headers + "\n" + m_body, true)) {
                lock_guard guard(mutex);
                response_clients.erase(client);
            }
        }).detach();
    }
}
