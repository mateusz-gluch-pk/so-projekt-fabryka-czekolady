//
// Created by mateusz on 26.11.2025.
//

#include "Deliverer.h"

#include <utility>

Deliverer::Deliverer(std::string name, ItemTemplate tpl, const Warehouse &dst, const Logger &log) :
    _name(std::move(name)),
    _tpl(std::move(tpl)),
    _log(log),
    _msq(log.key(), false),
    _running(true),
    _paused(false),
    _reloading(false) {
    _dst.emplace(dst.name(), dst.capacity(), &_log, false);
}

void Deliverer::run(ProcessStats &stats) {
    _reattach();

    while (_running) {
        if (_paused) {
            stats.state = PAUSED;
            sthr::sleep_for(stime::milliseconds(100));
            continue;
        }

        if (_reloading) {
            stats.state = RELOADING;
            _reload();
            _reloading = false;
            stats.reloads++;
        }

        stats.state = RUNNING;
        // _main is a <long> operation
        _main();
        stats.loops++;
    }
    stats.state = STOPPED;
}

void Deliverer::stop() { _running = false; }

void Deliverer::pause() { _paused = true; }

void Deliverer::resume() { _paused = false; }

void Deliverer::reload() { _reloading = true; }

void Deliverer::_main() const {
    sthr::sleep_for(stime::milliseconds(_tpl.delay_ms()));
    auto item = _tpl.get();
    _dst->add(item);
}

void Deliverer::_reload() {}
