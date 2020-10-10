//
// Created by Paker on 03/10/20.
//

#include "DeviceInfo.h"
#include "json.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

using namespace std;
using json = nlohmann::json;

void to_json(json &j, const DeviceInfo &i) {
    j = {{"type",    i.type},
         {"name",    i.name},
         {"appId",   i.appId},
         {"version", i.version}};
}

void to_json(json &j, const IPAddress &i) {
    j = {{"adapter", i.adapter},
         {"ip",      i.ip}};
}

vector<IPAddress> getIPAddress() {
    ifaddrs *ifap, *ifa;
    sockaddr_in *sa;
    vector<IPAddress> addresses;

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (sockaddr_in *) ifa->ifa_addr;
            string adp = ifa->ifa_name;
            string addr = inet_ntoa(sa->sin_addr);
            addresses.push_back({adp, addr});
        }
    }
    freeifaddrs(ifap);
    return addresses;
}
