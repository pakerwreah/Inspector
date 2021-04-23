//
// Created by Paker on 03/10/20.
//

#ifndef INSPECTOR_DEVICEINFO_H
#define INSPECTOR_DEVICEINFO_H

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

#endif //INSPECTOR_DEVICEINFO_H
