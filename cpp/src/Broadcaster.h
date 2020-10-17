//
// Created by Paker on 03/10/20.
//

#ifndef INSPECTOR_BROADCASTER_H
#define INSPECTOR_BROADCASTER_H

#include "DeviceInfo.h"
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
    std::thread *start(int port, const DeviceInfo &info);
    void setInterval(std::chrono::nanoseconds interval);
};

#endif //INSPECTOR_BROADCASTER_H
