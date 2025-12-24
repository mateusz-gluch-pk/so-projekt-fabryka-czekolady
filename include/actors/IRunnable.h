//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_IRUNNABLE_H
#define PROJEKT_IRUNNABLE_H

#include "processes/ProcessStats.h"
#include <string>

#include "logger/Logger.h"

class IRunnable {
    public:
        virtual ~IRunnable() = default;
        virtual void run(ProcessStats &stats, Logger &log) = 0; // fork
        virtual void stop() = 0; // SIGTERM
        virtual void pause() = 0; // SIGSTOP
        virtual void resume() = 0; // SIGCONT
        virtual void reload() = 0; // SIGHUP
        virtual const std::string &name() = 0;
};


#endif //PROJEKT_IRUNNABLE_H