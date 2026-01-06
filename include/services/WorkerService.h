//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_WORKERSERVICE_H
#define FACTORY_WORKERSERVICE_H
#include "actors/Worker.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"

// Data class for deliverer data
class WorkerStats {
public:
    WorkerStats() {};
    WorkerStats(std::string name, std::string in_name, std::string out_name, const Recipe &recipe, const ProcessStats *stats):
    name(std::move(name)),
    in_name(std::move(in_name)),
    out_name(std::move(out_name)),
    recipe(recipe),
    stats(stats)
    {}

    std::string name;
    std::string in_name;
    std::string out_name;
    Recipe recipe;

    // read only pointer to shm
    const ProcessStats *stats = nullptr;

    static std::vector<std::string> headers() {
        return std::vector<std::string>{
            "Name",
            "Input",
            "Output",
            "Recipe",
            "Status",
            "Loops",
            "Reloads",
            "PID"
        };
    }

    std::vector<std::string> row() const {
        return std::vector<std::string>{
            name,
            in_name,
            out_name,
            recipe.name(),
            state_to_string(stats->state),
            std::to_string(stats->loops),
            std::to_string(stats->reloads),
            std::to_string(stats->pid),
        };
    };
};

class WorkerService {
public:
    explicit WorkerService(Logger &_log, const bool debug = false): _log(_log), _debug(debug) {};
    ~WorkerService();

    WorkerStats *create(const std::string &name, const Recipe &recipe, Warehouse &in, Warehouse &out);
    void destroy(const std::string &name);

    WorkerStats *get(const std::string &name);
    std::vector<WorkerStats *> get_all();

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
    std::map<std::string, WorkerStats> _stats;
    bool _debug;
};

#endif //FACTORY_WORKERSERVICE_H