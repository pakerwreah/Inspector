//
// Created by Paker on 03/10/20.
//

#pragma once

#include "json.hpp"

struct IPAddress {
    std::string adapter;
    std::string ip;
};

struct DeviceInfo {
    std::string type;
    std::string name;
    std::string appId;
    std::string version;
};

void to_json(nlohmann::json &j, const DeviceInfo &i);
void to_json(nlohmann::json &j, const IPAddress &i);

std::vector<IPAddress> getIPAddress();
