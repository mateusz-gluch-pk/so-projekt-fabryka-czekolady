//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_DELIVERERSERVICE_H
#define FACTORY_DELIVERERSERVICE_H
#include <map>
#include <string>

#include "actors/Deliverer.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"

// Data class for deliverer data
class DelivererStats {
    public:
    DelivererStats() = default;
    DelivererStats(std::string name, std::string dst_name, const ItemTemplate &tpl, const ProcessStats *stats):
    name(std::move(name)),
    dst_name(std::move(dst_name)),
    tpl(tpl),
    stats(stats)
    {}

    std::string name;
    std::string dst_name;
    ItemTemplate tpl;

    // read only pointer to shm
    const ProcessStats *stats = nullptr;


    static std::vector<std::string> headers() {
        return std::vector<std::string>{
            "Name",
            "Output",
            "Item",
            "Base Delay [ms]",
            "Status",
            "Loops",
            "Reloads",
            "PID"
        };
    }

    std::vector<std::string> row() const {
        return std::vector<std::string>{
            name,
            dst_name,
            tpl.get().name(),
            std::to_string(tpl.base_delay_ms()),
            state_to_string(stats->state),
            std::to_string(stats->loops),
            std::to_string(stats->reloads),
            std::to_string(stats->pid),
        };
    };
};

class DelivererService {
public:
    explicit DelivererService(Logger &_log, const bool debug = false): _log(_log), _debug(debug) {};
    ~DelivererService();

    DelivererStats *create(const std::string &name, const ItemTemplate &tpl, Warehouse &dst);
    void destroy(const std::string &name);
    DelivererStats *get(const std::string &name);
    std::vector<DelivererStats *> get_all();

    void pause(const std::string &name);
    void resume(const std::string &name);
    void reload(const std::string &name);
    void reload_all();

private:
    static std::string _msg(const std::string &msg) {
        return "services/DeliverersService:\t" + msg;
    }

    Logger &_log;
    std::map<std::string, ProcessController *> _deliverers;
    std::map<std::string, DelivererStats> _stats;
    bool _debug;
};


#endif //FACTORY_DELIVERERSERVICE_H