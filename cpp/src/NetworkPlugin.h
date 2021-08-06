//
// Created by Paker on 09/11/19.
//

#pragma once

#include "Router.h"
#include "WebSocket.h"
#include <mutex>
#include <set>

class NetworkPlugin {
private:
    std::set<std::shared_ptr<WebSocket>> request_clients, response_clients;
    std::mutex mutex;

public:
    explicit NetworkPlugin(Router &router);

    bool isRequestConnected() const;
    bool isResponseConnected() const;
    bool isConnected() const;

    void sendRequest(const std::string &uid, const std::string &headers, const std::string &body);
    void sendResponse(const std::string &uid, const std::string &headers, const std::string &body, bool is_compressed = false);
};
