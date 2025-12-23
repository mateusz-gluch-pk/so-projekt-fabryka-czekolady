//
// Created by mateusz on 23.12.2025.
//

#ifndef FACTORY_PROCESSSTATS_H
#define FACTORY_PROCESSSTATS_H


enum ProcessState {
    CREATED = 0,
    RUNNING,
    PAUSED,
    RELOADING,
    STOPPED,
};

class ProcessStats {
    public:
        ProcessState state = CREATED;
        int loops = 0;
        int reloads = 0;
};


#endif //FACTORY_PROCESSSTATS_H