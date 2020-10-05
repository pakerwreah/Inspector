//
// Created by Paker on 03/10/20.
//

#include "Broadcaster.h"
#include "UDPSocket.h"
#include "DeviceInfo.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

Broadcaster::Broadcaster() : _stop(false) {}

void Broadcaster::stop() {
    _stop = true;
}

bool Broadcaster::broadcasting() const {
    return _broadcasting;
}

thread *Broadcaster::start(int port, const DeviceInfo &info) {
    _stop = false;
    return new thread([=] {
        vector<IPAddress> addresses;
        for (const IPAddress &addr : getIPAddress()) {
            addresses.push_back(addr);
        }
        json jdatagram(info);
        jdatagram["addresses"] = addresses;
        string datagram = jdatagram.dump();

        do {
            UDPSocket socket;
            if (socket.create() && socket.bind(port)) {
                _broadcasting = true;
                // for some reason the first broadcast always fails on iOS
                socket.broadcast(datagram);
                do {
                    if (!socket.broadcast(datagram)) {
                        printf("broadcast error %d\n", errno);
                        this_thread::sleep_for(1s);
                        break;
                    }
                    this_thread::sleep_for(5s);
                } while (!_stop);
                _broadcasting = false;
            } else {
                printf("broadcast create/bind error %d\n", errno);
                stop();
            }
        } while (!_stop);
    });
}
