//
// Created by Paker on 25/09/20.
//

#include "realtime.h"
#include "read_file.h"

Realtime::Realtime(Inspector &inspector) {
    inspector.addLivePlugin("realtime", "Realtime", [] {
        return read_file("../ext/realtime/realtime.html");
    });
}
