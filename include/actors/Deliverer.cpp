//
// Created by mateusz on 26.11.2025.
//

#include "Deliverer.h"

#include <utility>

#include "services/WarehouseService.h"

#define PROCESS_DIR "processes"

Deliverer::Deliverer(std::string name, ItemTemplate tpl, WarehouseService &svc, Logger &log, const bool child) :
    _name(std::move(name)),
    _tpl(std::move(tpl)),
    _svc(svc),
    _log(log),
    _sem_paused(make_key(PROCESS_DIR, _name, &log), &log, !child) {
    _log.info(_msg("Created").c_str());
}

void Deliverer::run(ProcessStats *stats) {
    _stats = stats;
    stats->pid = getpid();
    stats->state = RUNNING;

    while (true) {
        _sem_paused.lock();
        _sem_paused.unlock();

        // _main is a <long> operation
        _main();
        _log.debug(_msg("Loop completed").c_str());
        stats->loops++;
    }
}

void Deliverer::stop() {
    _log.info(_msg("Received SIGTERM - terminating").c_str());
    _stats->state = STOPPED;
    exit(0);
}

void Deliverer::pause() {
    _log.info(_msg("Received SIGUSR1 - pausing").c_str());
    if (_stats != nullptr) {
        _stats->state = PAUSED;
    }
    if (_sem_paused.value() != 0) {
        _sem_paused.lock();
    }
}

void Deliverer::resume() {
    _log.info(_msg("Received SIGCONT - resuming").c_str());
    if (_stats != nullptr) {
        _stats->state = RUNNING;
    }
    if (_sem_paused.value() != 1) {
        _sem_paused.unlock();
    }
}

void Deliverer::reload() {
    _log.info(_msg("Received SIGHUP - reloading").c_str());
    if (_stats != nullptr) {
        _stats->state = RELOADING;
    }
    try {
        _reattach(_log);
    } catch (std::exception &e) {
        _log.warn(e.what());
        pause();
    }
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

void Deliverer::_reattach(Logger &log) {
    _log = log;
    auto item = _tpl.get();
    _svc.destroy(item->name());
    if (const auto wh = _svc.attach(item->name(), item->size()); wh == nullptr) {
        _log.fatal(_msg("Cannot reattach to warehouse Item<%d>(\"%s\")").c_str(), item->name().c_str(), item->size());
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
