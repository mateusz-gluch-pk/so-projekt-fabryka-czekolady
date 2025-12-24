//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_DELIVERER_H
#define PROJEKT_DELIVERER_H
#include <atomic>
#include <thread>

#include "IRunnable.h"
#include "ipcs/MessageQueue.h"
#include "objects/ItemTemplate.h"
#include "stations/Warehouse.h"
#include "logger/Logger.h"
#include "processes/ProcessStats.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;


class Deliverer : public IRunnable {
    public:
        Deliverer(std::string name, ItemTemplate tpl, Warehouse &dst, Logger &log);

        void run(ProcessStats &stats, Logger &log) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;
        const std::string &name() override { return _name; }

    private:
        void _main() const;
        void _reload();

        void _reattach(Logger &log) {
            _log = log;
            _dst.emplace(_dst->name(), _dst->capacity(), &log, false);
        }

        [[nodiscard]] std::string _msg(const std::string &msg) const {
            return "Deliverer " + _name + ": " + msg;
        }
        std::string _name;
        ItemTemplate _tpl;

        Logger &_log;
        std::optional<Warehouse> _dst;
        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_DELIVERER_H