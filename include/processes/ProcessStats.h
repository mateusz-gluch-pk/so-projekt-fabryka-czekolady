//
// Created by mateusz on 23.12.2025.
//

#ifndef FACTORY_PROCESSSTATS_H
#define FACTORY_PROCESSSTATS_H
#include <string>


enum ProcessState {
    CREATED = 0,
    RUNNING,
    PAUSED,
    RELOADING,
    STOPPED,
};

inline std::string state_to_string(const ProcessState state) {
    switch (state) {
        case CREATED: return "CREATED";
        case RUNNING: return "RUNNING";
        case PAUSED: return "PAUSED";
        case RELOADING: return "RELOADING";
        case STOPPED: return "STOPPED";
        default: return "UNKNOWN";
    }
};

class ProcessStats {
    public:
        ProcessState state = CREATED;
        int loops = 0;
        int reloads = 0;
        int pid = 0;
};


#endif //FACTORY_PROCESSSTATS_H