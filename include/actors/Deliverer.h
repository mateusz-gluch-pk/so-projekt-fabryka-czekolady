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

namespace stime = std::chrono;
namespace sthr = std::this_thread;


class Deliverer : public IRunnable {
    public:
        Deliverer(const ItemTemplate &tpl, Warehouse *dst, Logger *log);;
        ~Deliverer() override {}

        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;

    private:
        void _main() const;
        void _reload();

        void _reattach() {
            _log->setQueue(static_cast<IQueue<Message>>(MessageQueue<Message>::attach(_log->key())));
            _dst->reattach(_log);
        }

        ItemTemplate _tpl;
        Warehouse *_dst;
        Logger *_log;

        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_DELIVERER_H