//
// Created by mateusz on 26.11.2025.
//

#include "Worker.h"
#include "services/WarehouseService.h"

Worker::Worker(std::string name, std::unique_ptr<Recipe> recipe, WarehouseService &svc, Logger &log):
    _name(std::move(name)),
    _recipe(std::move(recipe)),
    _inventory(std::vector<std::unique_ptr<IItem>>()),
    _log(log),
    _running(true),
    _paused(false),
    _reloading(false),
    _svc(svc) {
    _log.info(_msg("Created").c_str());
}

void Worker::run(ProcessStats *stats) {
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
        _main();
        _log.debug(_msg("Loop completed").c_str());
        stats->loops++;
    }
    stats->state = STOPPED;
}

void Worker::stop() {
    _log.info(_msg("Received SIGTERM - terminating").c_str());
    _running = false;
}

void Worker::pause() {
    _log.info(_msg("Received SIGUSR1 - pausing").c_str());
    _paused = true;
}

void Worker::resume() {
    _log.info(_msg("Received SIGCONT - resuming").c_str());
    _paused = false;
}

void Worker::reload() {
    _log.info(_msg("Received SIGHUP - reloading").c_str());
    _reloading = true;
}

void Worker::_main() {
    std::unique_ptr<IItem> output = nullptr;

    // sleep for a tick (optional)
    sthr::sleep_for(stime::milliseconds(WORKER_TICK_DELAY));

    _log.debug(_msg("Producing " + _recipe->name()).c_str());
    const std::string missing = _recipe->try_produce(_inventory, output);

    if (missing.empty() && output != nullptr) {
        _log.info(_msg("Produced " + output->name()).c_str());

        auto wh = _svc.get(output->name());
        if (wh == nullptr) {
            _log.error(_msg("Cannot store " + output->name()).c_str());
            return;
        }
        wh->add(*output.get());
        return;
    }

    _log.debug(_msg("Failed to produce " + _recipe->name() + ", missing " + missing).c_str());

    auto wh = _svc.get(missing);
    if (wh == nullptr) {
        _log.error(_msg("No warehouse found for " + missing).c_str());
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

// Use RELOAD to make the worker check its warehouses - if not available; turn into IDLE
void Worker::_reload() {
    try {
        _reloading = false;
        _reattach(_log);
    } catch (std::exception &e) {
        _paused = true;
        _log.warn(e.what());
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