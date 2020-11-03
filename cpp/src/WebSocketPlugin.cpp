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
            shared_ptr client = make_shared<WebSocket>(request.client);
            clients.insert({plugin, client});
        }
        return response;
    });
}

bool WebSocketPlugin::isConnected() const {
    return clients.size();
}

void WebSocketPlugin::sendMessage(const string &key, const string &message) {
    lock_guard guard(mutex);
    auto range = clients.equal_range(key);
    auto it = range.first;
    while (it != range.second) {
        shared_ptr client = it->second;
        string m_message = message;
        if (client->send(m_message, false)) {
            it++;
        } else {
            it = clients.erase(it);
        }
    }
}
