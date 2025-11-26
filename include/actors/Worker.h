//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKER_H
#define PROJEKT_WORKER_H
#include "Process.h"
#include "Warehouse.h"
#include "Workstation.h"


class Worker {
    public:
        Worker();

        // SIG process control (from outside)
        // Process run();
        int run();
        void stop(); //SIGTERM
        void pause(); //SIGSTOP
        void resume(); //SIGCONT
        void reload(); //SIGHUP

    private:
        std::string _name;
        int _pid;
        int _ppid;

        // Process _process;
        Warehouse *_in;
        Warehouse *_out;
        Workstation *_station;

        void _loop();
};


#endif //PROJEKT_WORKER_H