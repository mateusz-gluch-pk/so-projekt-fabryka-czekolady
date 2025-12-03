//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKER_H
#define PROJEKT_WORKER_H
#include "IRunnable.h"
#include "Warehouse.h"
#include "Workstation.h"
#include "logger/Logger.h"


class Worker : public IRunnable {
    public:
        Worker(Logger *log): _log(log) {};
        ~Worker();

        // SIG process control (from outside)
        // Process run();
        void run() override;
        void stop() override; //SIGTERM
        void pause() override; //SIGSTOP
        void resume() override; //SIGCONT
        void reload() override; //SIGHUP

    private:
        Logger *_log;
        Warehouse *_in;
        Warehouse *_out;
        Workstation *_station;
};


#endif //PROJEKT_WORKER_H