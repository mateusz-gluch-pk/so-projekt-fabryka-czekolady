//
// Created by mateusz on 26.11.2025.
//

#include "Worker.h"

Worker::Worker(Logger *log, Warehouse *in, Warehouse *out, Workstation *station):
    _log(log),
    _in(in),
    _out(out),
    _station(station),
    _recipe(station->recipe()),
    _inventory(std::vector<Item>()),
    _running(true),
    _paused(false),
    _reloading(false) {
}

void Worker::run() {
    while (_running) {
        if (_paused) {
            sthr::sleep_for(stime::milliseconds(100));
            continue;
        }

        if (_reloading) {
            _reload();
            _reloading = false;
        }

        _main();
    }
}

void Worker::stop() {_running = false;}

void Worker::pause() {_paused = true;}

void Worker::resume() {_paused = false;}

void Worker::reload() {_reloading = true;}

void Worker::_main() {
    auto *output = new Item;
    const std::string missing = _recipe.try_produce(_inventory, output);

    if (output->count() != 0) {
        _out->add(*output);
        _inventory.clear();
        delete output;
        output = nullptr;
        return;
    }

    _in->get(missing, output);
    if (output->count() == 0) {
        _inventory.push_back(*output);
    }

    delete output;
    output = nullptr;
}

void Worker::_reload() {}
