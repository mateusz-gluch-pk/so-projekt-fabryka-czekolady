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
        Deliverer(std::string name, ItemTemplate tpl, const Warehouse &dst, const Logger &log);

        void run(ProcessStats &stats) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;
        const std::string &name() override { return _name; }

    private:
        void _main() const;
        void _reload();

        void _reattach() {
            _msq = MessageQueue<Message>(_log.key(), false);
            _log.setQueue(&_msq);
            _dst.emplace(_dst->name(), _dst->capacity(), &_log, false);
        }

        std::string _name;
        ItemTemplate _tpl;
        ProcessStats _stats;

        MessageQueue<Message> _msq;
        std::optional<Warehouse> _dst;
        Logger _log;

        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_DELIVERER_H