//
// Created by mateusz on 26.11.2025.
//

#include "Deliverer.h"

#include <utility>

Deliverer::Deliverer(std::string name, ItemTemplate tpl, Warehouse &dst, Logger &log) :
    _name(std::move(name)),
    _tpl(std::move(tpl)),
    _log(log),
    _running(true),
    _paused(false),
    _reloading(false) {
    _log.info(_msg("Trying to attach warehouse").c_str());
    _dst.emplace(dst.name(), dst.capacity(), &_log, false);
}

void Deliverer::run(ProcessStats &stats, Logger &log) {
    _reattach(log);

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
        _log.debug(_msg("Loop completed").c_str());
        stats.loops++;
    }
    stats.state = STOPPED;
}

void Deliverer::stop() {
    _log.info(_msg("Received SIGTERM - pausing").c_str());
    _running = false;
}

void Deliverer::pause() {
    _log.info(_msg("Received SIGSTOP - pausing").c_str());
    _paused = true;
}

void Deliverer::resume() {
    _log.info(_msg("Received SIGCONT - resuming").c_str());
    _paused = false;
}

void Deliverer::reload() {
    _log.info(_msg("Received SIGHUP - reloading").c_str());
    _reloading = true;
}

void Deliverer::_main() const {
    sthr::sleep_for(stime::milliseconds(_tpl.delay_ms()));
    _log.info(_msg("Delivering item to warehouse").c_str());
    auto item = _tpl.get();
    _dst->add(item);
}

void Deliverer::_reload() {}
