//
// Created by Paker on 03/10/20.
//

#include "DeviceInfo.h"
#include "json.hpp"
#include "util.h"

using namespace std;
using json = nlohmann::json;

void to_json(json &j, const DeviceInfo &i) {
    j = {{"type", i.type},
         {"name", i.name}};
}

void to_json(json &j, const IPAddress &i) {
    j = {{"adapter", i.adapter},
         {"ip",      i.ip}};
}

vector<IPAddress> getIPAddress() {
    ifaddrs *ifap, *ifa;
    sockaddr_in *sa;
    vector<IPAddress> addresses;
    auto sw = util::startsWith;

    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (sockaddr_in *) ifa->ifa_addr;
            string adp = ifa->ifa_name;
            string addr = inet_ntoa(sa->sin_addr);

            if (!sw(adp, "lo") && !sw(adp, "vbox") && !sw(adp, "pdp_ip")) {
                addresses.push_back({adp, addr});
            }
        }
    }
    freeifaddrs(ifap);
    return addresses;
}
