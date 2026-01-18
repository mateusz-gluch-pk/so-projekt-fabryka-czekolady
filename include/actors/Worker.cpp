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

void Worker::run(ProcessStats *stats) {
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
    Item output;

    // sleep for a tick (optional)
    sthr::sleep_for(stime::milliseconds(WORKER_TICK_DELAY));

    _log.debug(_msg("Producing " + _recipe.name()).c_str());
    const std::string missing = _recipe.try_produce(_inventory, &output);

    if (output.count() != 0) {
        _log.info(_msg("Produced " + output.name()).c_str());
        _out->add(output);
        return;
    }

    _log.debug(_msg("Failed to produce " + _recipe.name() + ", missing " + missing).c_str());
    _in->get(missing, &output);
    if (output.count() != 0) {
        _log.info(_msg("Retrieved " + output.name()).c_str());
        _inventory.push_back(output);
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

    args.push_back("--in_name");
    args.push_back(_in->name());

    args.push_back("--in_cap");
    args.push_back(std::to_string(_in->capacity()));

    args.push_back("--out_name");
    args.push_back(_out->name());

    args.push_back("--out_cap");
    args.push_back(std::to_string(_out->capacity()));

    args.push_back("--recipe_output");
    args.push_back(_recipe.name());

    nlohmann::json j = _recipe.inputs();
    args.push_back("--recipe_inputs");
    args.push_back(j.dump());

    args.push_back("--log_key");
    args.push_back(std::to_string(_log.key()));

    return args;
}