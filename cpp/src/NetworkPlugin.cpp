//
// Created by Paker on 09/11/19.
//

#include "NetworkPlugin.h"
#include "compress.hpp"
#include "util.h"

NetworkPlugin::NetworkPlugin(Router &router) {
    router.get("/network/request", [this](const Request &request, const Params &) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            std::lock_guard guard(mutex);
            request_clients.insert(make_unique<WebSocket>(request.client));
        }
        return response;
    });

    router.get("/network/response", [this](const Request &request, const Params &) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            std::lock_guard guard(mutex);
            response_clients.insert(make_unique<WebSocket>(request.client));
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

std::string pack(const std::array<std::string, 3> &data) {
    return util::join(data, '\n');
}

void NetworkPlugin::sendRequest(const std::string &uid, const std::string &headers, const std::string &body) {
    std::lock_guard guard(mutex);

    for (auto it = request_clients.begin(); it != request_clients.end();) {
        const std::string c_body = gzip::compress(body.c_str(), body.size());

        if ((*it)->send(pack({uid, headers, c_body}), true)) {
            ++it;
        } else {
            it = request_clients.erase(it);
        }
    }
}

void NetworkPlugin::sendResponse(
    const std::string &uid,
    const std::string &headers,
    const std::string &body,
    bool is_compressed
) {
    std::lock_guard guard(mutex);

    for (auto it = response_clients.begin(); it != response_clients.end();) {
        const std::string c_body = is_compressed ? body : gzip::compress(body.c_str(), body.size());

        if ((*it)->send(pack({uid, headers, c_body}), true)) {
            ++it;
        } else {
            it = response_clients.erase(it);
        }
    }
}
