//
// Created by mateusz on 26.11.2025.
//

#include "Worker.h"
#include "services/WarehouseService.h"

#define PROCESS_DIR "processes"

namespace sthr = std::this_thread;
namespace stime = std::chrono;

Worker::Worker(std::string name, std::unique_ptr<Recipe> recipe, WarehouseService &svc, Logger &log, bool child):
    _name(std::move(name)),
    _recipe(std::move(recipe)),
    _inventory(std::vector<std::unique_ptr<IItem>>()),
    _log(log),
    _svc(svc),
    _sem_paused(make_key(PROCESS_DIR, _name, &log), &log, !child) {
    _log.info(_msg("Created").c_str());
}

[[noreturn]] void Worker::run(ProcessStats *stats) {
    _stats = stats;
    stats->state = RUNNING;
    stats->pid = getpid();

    while (true) {
        _sem_paused.lock();
        _sem_paused.unlock();

        _main();
        _log.debug(_msg("Loop completed").c_str());
        stats->loops++;
    }
}

void Worker::stop() {
    _log.info(_msg("Received SIGTERM - terminating").c_str());
    if (_stats != nullptr) {
        _stats->state = STOPPED;
    }
    exit(0);
}

void Worker::pause() {
    _log.info(_msg("Received SIGUSR1 - pausing").c_str());
    if (_stats != nullptr) {
        if (_stats->state == RUNNING) {
            _sem_paused.lock();
        }
        _stats->state = PAUSED;
    }
}

void Worker::resume() {
    _log.info(_msg("Received SIGCONT - resuming").c_str());
    if (_stats != nullptr) {
        if (_stats->state == PAUSED) {
            _sem_paused.unlock();
        }
        _stats->state = RUNNING;
    }
}

void Worker::reload() {
    _log.info(_msg("Received SIGHUP - reloading").c_str());
    if (_stats != nullptr) {
        resume();
    }
    try {
        _reattach(_log);
    } catch (std::exception &e) {
        _log.warn(e.what());
        pause();
    }
}

void Worker::_main() {
    std::unique_ptr<IItem> output = nullptr;

    _log.debug(_msg("Producing " + _recipe->name()).c_str());
    const std::string missing = _recipe->try_produce(_inventory, output);

    if (missing.empty() && output != nullptr) {
        _log.info(_msg("Produced " + output->name()).c_str());

        auto wh = _svc.get(output->name());
        if (wh == nullptr) {
            _log.error(_msg("Cannot store " + output->name()).c_str());
            pause();
            return;
        }
        wh->add(*output);
        return;
    }

    _log.debug(_msg("Failed to produce " + _recipe->name() + ", missing " + missing).c_str());

    auto wh = _svc.get(missing);
    if (wh == nullptr) {
        _log.error(_msg("No warehouse found for " + missing).c_str());
        pause();
        return;
    }

    auto fetched = wh->get(missing);
    if (fetched != nullptr) {
        _log.info(_msg("Retrieved " + fetched->name()).c_str());
        _inventory.push_back(std::move(fetched));
    } else {
        _log.warn(_msg("Failed to retrieve " + missing).c_str());
    }
}


std::string Worker::_msg(const std::string &msg) const {
    return "actors/Worker/" + _name + ":\t" + msg;
}

std::vector<std::string> Worker::argv() {
    auto args = std::vector<std::string>();

    args.push_back("/proc/self/exe");
    args.push_back("--worker");
    args.push_back("Worker");

    args.push_back("--name");
    args.push_back(_name);

    args.push_back("--output_name");
    args.push_back(_recipe->name());

    args.push_back("--input_names");
    args.push_back(_recipe->input_names());

    args.push_back("--input_sizes");
    args.push_back(_recipe->input_sizes());

    args.push_back("--log_key");
    args.push_back(std::to_string(_log.key()));

    return args;
}