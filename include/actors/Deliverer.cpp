//
// Created by mateusz on 26.11.2025.
//

#include "Deliverer.h"

#include <utility>

Deliverer::Deliverer(std::string name, ItemTemplate tpl, Warehouse &dst, Logger &log, bool child) :
    _name(std::move(name)),
    _tpl(std::move(tpl)),
    _log(log),
    _running(true),
    _paused(false),
    _reloading(false),
    _dst_name(dst.name()),
    _dst_capacity(dst.capacity()) {
    if (child) {
        _dst.emplace(_dst_name, _dst_capacity, &_log, false);
    }
    _log.info(_msg("Created").c_str());
}

void Deliverer::run(ProcessStats *stats) {
    stats->pid = getpid();
    // _reattach(log);

    while (_running) {
        if (_paused) {
            stats->state = PAUSED;
            sthr::sleep_for(stime::milliseconds(10));
            continue;
        }

        if (_reloading) {
            stats->state = RELOADING;
            _reload();
            stats->reloads++;
            continue;
        }

        stats->state = RUNNING;
        // _main is a <long> operation
        _main();
        _log.debug(_msg("Loop completed").c_str());
        stats->loops++;
    }
    stats->state = STOPPED;
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

void Deliverer::_reattach(Logger &log) {
    _log = log;
    _dst.emplace(_dst->name(), _dst->capacity(), &log, false);
}

std::string Deliverer::_msg(const std::string &msg) const {
    return "actors/Deliverer/" + _name + ":\t" + msg;
}

std::vector<std::string> Deliverer::argv() {
    auto args = std::vector<std::string>();

    args.push_back("/proc/self/exe");
    args.push_back("--worker");
    args.push_back("Deliverer");

    args.push_back("--name");
    args.push_back(_name);

    args.push_back("--dst_name");
    args.push_back(_dst_name);

    args.push_back("--item_name");
    args.push_back(_tpl.get().name());

    args.push_back("--log_key");
    args.push_back(std::to_string(_log.key()));

    args.push_back("--dst_cap");
    args.push_back(std::to_string(_dst_capacity));

    args.push_back("--item_delay");
    args.push_back(std::to_string(_tpl.base_delay_ms()));

    args.push_back("--item_size");
    args.push_back(std::to_string(_tpl.get().size()));

    return args;
}
