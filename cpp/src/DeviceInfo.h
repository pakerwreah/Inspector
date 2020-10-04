//
// Created by Paker on 03/10/20.
//

#ifndef INSPECTOR_DEVICEINFO_H
#define INSPECTOR_DEVICEINFO_H

#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include "json.hpp"

struct IPAddress {
    std::string adapter;
    std::string ip;
};

struct DeviceInfo {
    std::string type;
    std::string name;
};

void to_json(nlohmann::json &j, const DeviceInfo &i);
void to_json(nlohmann::json &j, const IPAddress &i);

std::vector<IPAddress> getIPAddress();

#endif //INSPECTOR_DEVICEINFO_H
