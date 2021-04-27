//
// Created by Paker on 24/10/20.
//

#include "usage.h"
#include "read_file.h"

using namespace std;
using nlohmann::json;

Usage::Usage(Inspector &inspector) {
    inspector.addLivePlugin("usage", "Usage", [] {
        return read_file("../ext/usage/usage.html");
    });

    thread([&] {
        float cpu_usage = 50, memory_used = 50, memory_max = 512;
        unsigned int seed = 0;
        while (true) {
            cpu_usage = fmin(100, fmax(0, cpu_usage + rand_r(&seed) % 100 - 50));
            memory_used = fmin(memory_max, fmax(0, memory_used + rand_r(&seed) % 100 - 50));
            json cpu = {
                    {"usage", cpu_usage}
            };
            json memory = {
                    {"used", memory_used},
                    {"max",  memory_max}
            };
            json data = {
                    {"cpu",    cpu},
                    {"memory", memory}
            };
            inspector.sendMessage("usage", data.dump());
            this_thread::sleep_for(1s);
        }
    }).detach();
}
