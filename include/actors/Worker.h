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

namespace stime = std::chrono;
namespace sthr = std::this_thread;

class Worker : public IRunnable {
    public:
        Worker(Logger *log, Warehouse *in, Warehouse *out, Workstation *station);
        ~Worker() override {}

        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;

    private:
        void _main();
        void _reload();

        void _reattach() {
            _log->setQueue(static_cast<IQueue<Message>>(MessageQueue<Message>::attach(_log->key())));
            _in->reattach(_log);
            _out->reattach(_log);
        };

        Logger *_log;
        Warehouse *_in;
        Warehouse *_out;
        Workstation *_station;

        Recipe _recipe;
        std::vector<Item> _inventory;
        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_WORKER_H