//
// Created by Paker on 25/09/20.
//

#include "WebSocketPlugin.h"

WebSocketPlugin::WebSocketPlugin(Router &router) {
    router.get("/plugins/ws/{key}", [this](const Request &request, const Params &params) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            std::lock_guard guard(mutex);
            std::string plugin = params.at("key");
            clients.insert({
                plugin,
                make_unique<WebSocket>(request.client)
            });
        }
        return response;
    });
}

bool WebSocketPlugin::isConnected() const {
    return !clients.empty();
}

void WebSocketPlugin::sendMessage(const std::string &key, const std::string &message) {
    std::lock_guard guard(mutex);
    for (auto [it, end] = clients.equal_range(key); it != end;) {
        if (it->second->send(message, false)) {
            ++it;
        } else {
            it = clients.erase(it);
        }
    }
}
