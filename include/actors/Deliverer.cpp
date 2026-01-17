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
    _dst.emplace(dst.name(), dst.capacity(), &_log, false);
    _log.info(_msg("Created").c_str());
}

void Deliverer::run(ProcessStats &stats, Logger &log) {
    stats.pid = getpid();
    _reattach(log);

    while (_running) {
        if (_paused) {
            stats.state = PAUSED;
            sthr::sleep_for(stime::milliseconds(10));
            continue;
        }

        if (_reloading) {
            stats.state = RELOADING;
            // sthr::sleep_for(stime::milliseconds(100));
            _reload();
            stats.reloads++;
            continue;
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
    _log.info(_msg("Received SIGTERM - terminating").c_str());
    _running = false;
}

void Deliverer::pause() {
    _log.info(_msg("Received SIGUSR1 - pausing").c_str());
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
    _log.debug(_msg("Delivering item to warehouse").c_str());
    auto item = _tpl.get();
    _dst->add(item);
    sthr::sleep_for(stime::milliseconds(_tpl.delay_ms()));
}

void Deliverer::_reload() {
    try {
        _reloading = false;
        _reattach(_log);
    } catch (std::exception &e) {
        _paused = true;
        _log.warn(e.what());
    }
}
