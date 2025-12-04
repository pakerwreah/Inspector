//
// Created by Paker on 25/09/20.
//

#include "WebSocketPlugin.h"

using namespace std;

WebSocketPlugin::WebSocketPlugin(Router &router) {
    router.get("/plugins/ws/{key}", [this](const Request &request, const Params &params) {
        Response response = WebSocket::handshake(request);
        if (response.code == 101) {
            lock_guard guard(mutex);
            string plugin = params.at("key");
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

void WebSocketPlugin::sendMessage(const string &key, const string &message) {
    lock_guard guard(mutex);
    for (auto [it, end] = clients.equal_range(key); it != end;) {
        if (it->second->send(message, false)) {
            ++it;
        } else {
            it = clients.erase(it);
        }
    }
}
