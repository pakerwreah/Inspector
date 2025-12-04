//
// Created by Paker on 03/10/20.
//

#include "Broadcaster.h"
#include "UDPSocket.h"
#include "json.hpp"

using namespace std::chrono_literals;
using nlohmann::json;

Broadcaster::Broadcaster() : _stop(false), _broadcasting(false), _error(0), interval(5s) {}

int Broadcaster::error() const {
    return _error;
}

void Broadcaster::stop() {
    _stop = true;
}

bool Broadcaster::broadcasting() const {
    return _broadcasting;
}

std::thread *Broadcaster::start(int port, const DeviceInfo &info, timeval timeout) {
    _stop = false;
    return new std::thread([=, this] {
        std::vector<IPAddress> addresses;
        for (const IPAddress &addr : getIPAddress()) {
            addresses.push_back(addr);
        }
        json jdatagram(info);
        jdatagram["addresses"] = addresses;
        std::string datagram = jdatagram.dump();

        do {
            UDPSocket socket;
            if (socket.create() && socket.bind(port)) {
                _broadcasting = true;
                do {
                    if (!socket.broadcast(datagram, timeout) && errno != EHOSTUNREACH) {
                        _error = errno;
                        _stop = true;
                    }
                    if (!_stop) {
                        std::this_thread::sleep_for(interval);
                    }
                } while (!_stop);
                _broadcasting = false;
            } else {
                _error = errno;
                _stop = true;
            }
        } while (!_stop);
    });
}

void Broadcaster::setInterval(std::chrono::nanoseconds interval) {
    this->interval = interval;
}
