//
// Created by Paker on 03/10/20.
//

#pragma once

#include "DeviceInfo.h"
#include <chrono>
#include <thread>

class Broadcaster {
private:
    bool _stop;
    bool _broadcasting;
    int _error;
    std::chrono::nanoseconds interval;

public:
    Broadcaster();

    int error() const;
    void stop();
    bool broadcasting() const;
    std::thread *start(int port, const DeviceInfo &info, timeval timeout);
    void setInterval(std::chrono::nanoseconds interval);
};
