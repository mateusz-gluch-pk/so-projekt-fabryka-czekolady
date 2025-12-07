//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SUPERVISOR_H
#define PROJEKT_SUPERVISOR_H
#include <vector>

#include "Deliverer.h"
#include "Worker.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"


class Supervisor {
    public:
    Supervisor();
    ~Supervisor();

    void run();
    void stop();
    void pause();
    void resume();
    void reload();

    private:
    Logger *_log;
    std::vector<ProcessController *> _workers;
    std::vector<ProcessController *> _deliverers;

};


#endif //PROJEKT_SUPERVISOR_H