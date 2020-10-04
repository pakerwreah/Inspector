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
    return new thread([=] {
        vector<IPAddress> addresses;
        for (const IPAddress &addr : getIPAddress()) {
            addresses.push_back(addr);
        }
        json jdatagram(info);
        jdatagram["addresses"] = addresses;
        string datagram = jdatagram.dump();

        UDPSocket socket;
        if (socket.create() && socket.bind(port)) {
            _broadcasting = true;
            const int max_tries = 3;
            for (int i = 0; !_stop; i++) {
                if (i == max_tries) {
                    printf("broadcast error %d\n", errno);
                    stop();
                } else if (socket.broadcast(datagram)) {
                    i = -1;
                    this_thread::sleep_for(5s);
                }
            }
            _broadcasting = false;
        }
    });
}
