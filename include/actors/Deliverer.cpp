//
// Created by mateusz on 26.11.2025.
//

#include "Deliverer.h"

#include <utility>

#include "services/WarehouseService.h"

Deliverer::Deliverer(std::string name, ItemTemplate tpl, WarehouseService &svc, Logger &log) :
    _name(std::move(name)),
    _tpl(std::move(tpl)),
    _svc(svc),
    _log(log),
    _running(true),
    _paused(false),
    _reloading(false) {
    _log.info(_msg("Created").c_str());
}

void Deliverer::run(ProcessStats *stats) {
    stats->pid = getpid();

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
    auto dst = _svc.get(item->name());
    if (dst != nullptr) {
        dst->add(*item.get());
    } else {
        _log.error(_msg("Could not find dst warehouse").c_str());
    }
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
    auto item = _tpl.get();
    _svc.destroy(item->name());
    auto wh = _svc.attach(item->name(), item->size());
    if (wh == nullptr) {
        _log.error(_msg("Cannot reattach to warehouse Item<%d>(\"%s\")").c_str(), item->name().c_str(), item->size());
    }
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

    const auto i = _tpl.get();
    args.push_back("--item_name");
    args.push_back(i->name());

    args.push_back("--item_size");
    args.push_back(std::to_string(i->size()));

    args.push_back("--item_delay");
    args.push_back(std::to_string(_tpl.base_delay_ms()));

    args.push_back("--log_key");
    args.push_back(std::to_string(_log.key()));

    return args;
}
