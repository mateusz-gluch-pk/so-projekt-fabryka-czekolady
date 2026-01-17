//
// Created by mateusz on 27.12.2025.
//

#include "WorkerService.h"

std::vector<std::string> WorkerStats::headers() {
    return std::vector<std::string>{
        "Name",
        "Input",
        "Output",
        "Recipe",
        "Status",
        "Loops",
        "Reloads",
        "PID"
    };
}

std::vector<std::string> WorkerStats::row() const {
    return std::vector<std::string>{
        name,
        in_name,
        out_name,
        recipe.name(),
        state_to_string(stats->state),
        std::to_string(stats->loops),
        std::to_string(stats->reloads),
        std::to_string(stats->pid),
    };
}

WorkerService::~WorkerService() {
    for (const auto &pair: _workers) {
        if (pair.second->stats()->state != STOPPED) {
            pair.second->stop();
        }
        delete pair.second;
    }
    _workers.clear();
    _stats.clear();
}

WorkerStats *WorkerService::
create(const std::string &name, const Recipe &recipe, Warehouse &in, Warehouse &out) {
    if (_workers.contains(name)) {
        _log.error(_msg("Worker exists: " + name).c_str());
        return nullptr;
    }

    try {
        auto w = std::make_unique<Worker>(name, recipe, in, out, _log);
        const auto pw = new ProcessController(std::move(w), _log, true, _debug);
        pw->run();
        _workers[name] = pw;
        _stats[name] = WorkerStats(name, in.name(), out.name(), recipe, pw->stats());
        _log.info(_msg("Created worker: " + name).c_str());
        return &_stats[name];
    } catch (...) {
        _log.error(_msg("Failed to create worker: " + name).c_str());
        return nullptr;
    }
}

void WorkerService::destroy(const std::string &name) {
    const auto it = _workers.find(name);
    if (it == _workers.end()) {
        _log.error(_msg("Worker not found: " + name).c_str());
        return;
    }

    it->second->stop();
    _log.info(_msg("(Soft) Deleted worker: " + name).c_str());
}

WorkerStats *WorkerService::get(const std::string &name) {
    const auto it = _stats.find(name);
    if (it == _stats.end()) {
        _log.error(_msg("Worker not found: " + name).c_str());
        return nullptr;
    }

    _log.debug(_msg("Fetched worker: " + name).c_str());
    return &it->second;
}

// const ProcessStats * WorkerService::get_stats(const std::string &name) {
//     auto it = _workers.find(name);
//     if (it == _workers.end()) {
//         _log.error(_msg("Worker not found: " + name).c_str());
//         return nullptr;
//     }
//
//     _log.info(_msg("Fetched worker: " + name).c_str());
//     return it->second->stats();
// }

std::vector<WorkerStats *> WorkerService::get_all() {
    std::vector<WorkerStats *> result;
    result.reserve(_stats.size());

    for (auto &pair : _stats) {
        result.push_back(&pair.second);
    }

    _log.debug(_msg("Fetched all workers").c_str());
    return result;
}

// std::vector<const ProcessStats *> WorkerService::get_all_stats() {
//     std::vector<const ProcessStats *> result;
//     result.reserve(_workers.size());
//
//     for (auto &pair : _workers) {
//         result.push_back(pair.second->stats());
//     }
//
//     _log.info(_msg("Fetched all workers stats").c_str());
//     return result;
// }

void WorkerService::pause(const std::string &name) {
    const auto it = _workers.find(name);
    if (it == _workers.end()) {
        _log.error(_msg("Worker not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state == STOPPED) {
        _log.warn(_msg("Worker is terminated: " + name).c_str());
        return;
    }
    if (state != RUNNING) {
        _log.warn(_msg("Worker is not running: " + name).c_str());
    }

    _log.info(_msg("Pausing worker: " + name).c_str());
    it->second->pause();
    while (it->second->stats()->state != PAUSED) {
        sthr::sleep_for(std::chrono::milliseconds(10));
    }
}

void WorkerService::resume(const std::string &name) {
    const auto it = _workers.find(name);
    if (it == _workers.end()) {
        _log.error(_msg("Worker not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state == STOPPED) {
        _log.warn(_msg("Worker is terminated: " + name).c_str());
        return;
    }
    if (state != PAUSED) {
        _log.warn(_msg("Worker is not paused: " + name).c_str());
    }

    _log.info(_msg("Resuming worker: " + name).c_str());
    it->second->resume();
}

void WorkerService::reload(const std::string &name) {
    auto it = _workers.find(name);
    if (it == _workers.end()) {
        _log.error(_msg("Worker not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state == STOPPED) {
        _log.warn(_msg("Worker is terminated: " + name).c_str());
        return;
    }
    if (state != RUNNING) {
        _log.warn(_msg("Worker is not running: " + name).c_str());
    }

    _log.info(_msg("Reloading worker: " + name).c_str());
    it->second->reload();
}

void WorkerService::reload_all() {
    for (auto &pair : _workers) {
        reload(pair.first);
    }
}

std::string WorkerService::_msg(const std::string &msg) {
    return "services/WorkersService:\t" + msg;
}
