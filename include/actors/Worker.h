//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKER_H
#define PROJEKT_WORKER_H
#include <atomic>
#include <bits/this_thread_sleep.h>

#include "IRunnable.h"
#include "Warehouse.h"
#include "Workstation.h"
#include "logger/Logger.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;

class Worker : public IRunnable {
    public:
        Worker(Logger *log, Warehouse *in, Warehouse *out, Workstation *station):
            _log(log),
            _in(in),
            _out(out),
            _station(station),
            _running(true),
            _paused(false),
            _reloading(false) {
        }
        ~Worker() override {};

        void run() override {
            while (_running) {
                if (_paused) {
                    sthr::sleep_for(stime::milliseconds(100));
                    continue;
                }

                if (_reloading) {
                    _reload();
                    _reloading = false;
                }

                _main();
            }
        };
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;

    private:
        void _main() {

        }
        void _reload() {}

        Logger *_log;
        Warehouse *_in;
        Warehouse *_out;
        Workstation *_station;

        Recipe _recipe;


        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_WORKER_H