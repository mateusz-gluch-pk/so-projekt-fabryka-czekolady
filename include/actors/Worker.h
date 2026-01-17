//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKER_H
#define PROJEKT_WORKER_H
#include <atomic>
#include <bits/this_thread_sleep.h>

#include "IRunnable.h"
#include "ipcs/MessageQueue.h"
#include "stations/Warehouse.h"
#include "stations/Workstation.h"
#include "logger/Logger.h"
#include "objects/Recipe.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;

#define WORKER_TICK_DELAY 10

class Worker : public IRunnable {
    public:
        Worker(std::string name, Recipe recipe, Warehouse &in, Warehouse &out, Logger &log);

        void run(ProcessStats &stats, Logger &log) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;
        const std::string &name() override { return _name; }

    private:
        void _main();
        void _reload();

        void _reattach(Logger &log) {
            _log = log;
            _in.emplace(_in->name(), _in->capacity(), &log, false);
            _out.emplace(_out->name(), _out->capacity(), &log, false);
        };

        [[nodiscard]] std::string _msg(const std::string &msg) const {
            return "actors/Worker/" + _name + ":\t" + msg;
        }

        std::string _name;
        Recipe _recipe;

        std::optional<Warehouse> _in;
        std::optional<Warehouse> _out;

        Logger &_log;

        std::vector<Item> _inventory;
        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_WORKER_H