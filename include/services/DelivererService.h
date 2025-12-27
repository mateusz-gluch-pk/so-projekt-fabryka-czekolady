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


class DelivererService {
public:
    explicit DelivererService(Logger &_log): _log(_log) {};
    ~DelivererService();

    ProcessController *create(const std::string &name, const ItemTemplate &tpl, Warehouse &dst);
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
        return "services/DeliverersService:\t" + msg;
    }

    Logger &_log;
    std::map<std::string, ProcessController *> _deliverers;
};


#endif //FACTORY_DELIVERERSERVICE_H