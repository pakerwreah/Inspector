//
// Created by Paker on 09/11/19.
//

#include "NetworkPlugin.h"
#include "compress.hpp"
#include "util.h"

using namespace std;

NetworkPlugin::NetworkPlugin(Router &router) {
    router.get("/network/request", [this](const Request &request, const Params &) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            lock_guard guard(mutex);
            request_clients.insert(make_shared<WebSocket>(request.client));
        }
        return response;
    });

    router.get("/network/response", [this](const Request &request, const Params &) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            lock_guard guard(mutex);
            response_clients.insert(make_shared<WebSocket>(request.client));
        }
        return response;
    });
}

bool NetworkPlugin::isRequestConnected() const {
    return !request_clients.empty();
}

bool NetworkPlugin::isResponseConnected() const {
    return !response_clients.empty();
}

bool NetworkPlugin::isConnected() const {
    return isRequestConnected() && isResponseConnected();
}

string pack(const std::array<string, 3> &data) {
    return util::join(data, '\n');
}

void NetworkPlugin::sendRequest(const string &uid, const string &headers, const string &body) {
    lock_guard guard(mutex);

    for (auto it = request_clients.begin(); it != request_clients.end();) {
        shared_ptr client = *it;
        const string c_body = gzip::compress(body.c_str(), body.size());

        if (client->send(pack({uid, headers, c_body}), true)) {
            it++;
        } else {
            it = request_clients.erase(it);
        }
    }
}

void NetworkPlugin::sendResponse(const string &uid, const string &headers, const string &body, bool is_compressed) {
    lock_guard guard(mutex);

    for (auto it = response_clients.begin(); it != response_clients.end();) {
        shared_ptr client = *it;
        const string c_body = is_compressed ? body : gzip::compress(body.c_str(), body.size());

        if (client->send(pack({uid, headers, c_body}), true)) {
            it++;
        } else {
            it = response_clients.erase(it);
        }
    }
}
