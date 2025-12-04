//
// Created by mateusz on 26.11.2025.
//

#include "Deliverer.h"

Deliverer::Deliverer(const ItemTemplate &tpl, Warehouse *dst, Logger *log) :
    _tpl(tpl),
    _dst(dst),
    _log(log),
    _running(true),
    _paused(false),
    _reloading(false) {
}

void Deliverer::run() {
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

void Deliverer::stop() { _running = false; }

void Deliverer::pause() { _paused = true; }

void Deliverer::resume() { _paused = false; }

void Deliverer::reload() { _reloading = true; }

void Deliverer::_main() const {
    sthr::sleep_for(stime::milliseconds(_tpl.delay()));
    const auto item = _tpl.get();
    _dst->add(item);
}

void Deliverer::_reload() {}
