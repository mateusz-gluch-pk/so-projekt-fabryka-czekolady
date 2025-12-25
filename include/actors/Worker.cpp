//
// Created by mateusz on 26.11.2025.
//

#include "Worker.h"

Worker::Worker(std::string name, Recipe recipe, Warehouse &in, Warehouse &out, Logger &log):
    _name(std::move(name)),
    _recipe(std::move(recipe)),
    _inventory(std::vector<Item>()),
    _log(log),
    _running(true),
    _paused(false),
    _reloading(false) {
    _in.emplace(in.name(), in.capacity(), &_log, false);
    _out.emplace(out.name(), out.capacity(), &_log, false);
    _log.info(_msg("Created").c_str());
}

void Worker::run(ProcessStats &stats, Logger &log) {
    _reattach(log);

    while (_running) {
        if (_paused) {
            stats.state = PAUSED;
            sthr::sleep_for(stime::milliseconds(10));
            continue;
        }

        if (_reloading) {
            stats.state = RELOADING;
            _reload();
            _reloading = false;
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

void Worker::stop() {
    _log.info(_msg("Received SIGTERM - pausing").c_str());
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
    Item output;
    _log.info(_msg("Producing %s").c_str(), _recipe.name());
    const std::string missing = _recipe.try_produce(_inventory, &output);

    if (output.count() != 0) {
        _out->add(output);
        return;
    }

    _in->get(missing, &output);
    if (output.count() == 0) {
        _inventory.push_back(output);
    }
}

// Use RELOAD to make the worker check its warehouses - if not available; turn into IDLE
void Worker::_reload() {}
