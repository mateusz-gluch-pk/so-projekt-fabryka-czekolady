//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_WORKERSERVICE_H
#define FACTORY_WORKERSERVICE_H
#include "actors/Worker.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"

class WorkerService {
public:
    explicit WorkerService(Logger &_log): _log(_log) {};
    ~WorkerService();

    ProcessController *create(const std::string &name, const Recipe &recipe, Warehouse &in, Warehouse &out);
    void destroy(const std::string &name);

    ProcessController *get(const std::string &name);
    std::vector<ProcessController *> get_all();

    const ProcessStats *get_stats(const std::string &name);
    std::vector<const ProcessStats *> get_all_stats();

    void pause(const std::string &name);
    void resume(const std::string &name);
    void reload(const std::string &name);
    void reload_all();

private:
    static std::string _msg(const std::string &msg) {
        return "services/WorkersService:\t" + msg;
    }

    Logger &_log;
    std::map<std::string, ProcessController *> _workers;
};

#endif //FACTORY_WORKERSERVICE_H