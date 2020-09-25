//
// Created by Paker on 09/11/19.
//

#include "NetworkPlugin.h"
#include "compress.hpp"
#include <thread>

using namespace std;

NetworkPlugin::NetworkPlugin(Router *router) {
    router->get("/network/request", [this](const Request &request, const Params &) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            lock_guard guard(mutex);
            request_clients.insert(make_shared<WebSocket>(request.client));
        }
        return response;
    });

    router->get("/network/response", [this](const Request &request, const Params &) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            lock_guard guard(mutex);
            response_clients.insert(make_shared<WebSocket>(request.client));
        }
        return response;
    });
}

bool NetworkPlugin::isRequestConnected() const {
    return request_clients.size();
}

bool NetworkPlugin::isResponseConnected() const {
    return response_clients.size();
}

bool NetworkPlugin::isConnected() const {
    return isRequestConnected() && isResponseConnected();
}

void NetworkPlugin::sendRequest(const string &uid, const string &headers, const string &body) {
    lock_guard guard(mutex);
    auto it = request_clients.begin();
    while (it != request_clients.end()) {
        shared_ptr client = *it;
        const string m_body = gzip::compress(body.c_str(), body.size());
        if (client->send(uid + "\n" + headers + "\n" + m_body, true)) {
            it++;
        } else {
            it = request_clients.erase(it);
        }
    }
}

void NetworkPlugin::sendResponse(const string &uid, const string &headers, const string &body, bool compressed) {
    lock_guard guard(mutex);
    auto it = response_clients.begin();
    while (it != response_clients.end()) {
        shared_ptr client = *it;
        string m_body = body;
        if (!compressed) {
            m_body = gzip::compress(body.c_str(), body.size());
        }
        if (client->send(uid + "\n" + headers + "\n" + m_body, true)) {
            it++;
        } else {
            it = response_clients.erase(it);
        }
    }
}
