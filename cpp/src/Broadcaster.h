//
// Created by Paker on 03/10/20.
//

#ifndef INSPECTOR_BROADCASTER_H
#define INSPECTOR_BROADCASTER_H

#include "DeviceInfo.h"
#include <thread>

class Broadcaster {
    bool _stop;
    bool _broadcasting;

public:
    Broadcaster();

    void stop();
    bool broadcasting() const;
    std::thread *start(int port, const DeviceInfo &info);
};


#endif //INSPECTOR_BROADCASTER_H
