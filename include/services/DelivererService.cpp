//
// Created by mateusz on 27.12.2025.
//

#include "DelivererService.h"

DelivererService::~DelivererService() {
    for (auto &pair: _deliverers) {
        pair.second->stop();
        delete pair.second;
    }

    _deliverers.clear();
    _stats.clear();
}

DelivererStats *DelivererService::create(const std::string &name, const ItemTemplate &tpl, Warehouse &dst) {
    if (_deliverers.contains(name)) {
        _log.error(_msg("Deliverer exists: " + name).c_str());
        return nullptr;
    }

    try {
        auto d = std::make_unique<Deliverer>(name, tpl, dst, _log);
        const auto pd = new ProcessController(std::move(d), _log);
        pd->run();
        _deliverers[name] = pd;
        _stats[name] = DelivererStats(name, dst.name(), tpl, pd->stats());
        _log.info(_msg("Created deliverer: " + name).c_str());
        return &_stats[name];
    } catch (...) {
        _log.error(_msg("Failed to create deliverer: " + name).c_str());
        return nullptr;
    }
}

void DelivererService::destroy(const std::string &name) {
    _stats.erase(name);

    auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
        return;
    }

    it->second->stop();
    delete it->second;
    _deliverers.erase(it);

    _log.info(_msg("Deleted deliverer: " + name).c_str());
}

DelivererStats *DelivererService::get(const std::string &name) {
    auto it = _stats.find(name);
    if (it == _stats.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
        return nullptr;
    }

    _log.info(_msg("Fetched deliverer: " + name).c_str());
    return &it->second;
}

// const ProcessStats * DelivererService::get_stats(const std::string &name) {
//     auto it = _deliverers.find(name);
//     if (it == _deliverers.end()) {
//         _log.error(_msg("Deliverer not found: " + name).c_str());
//         return nullptr;
//     }
//
//     _log.info(_msg("Fetched deliverer: " + name).c_str());
//     return it->second->stats();
// }

std::vector<DelivererStats *> DelivererService::get_all() {
    std::vector<DelivererStats *> result;
    result.reserve(_stats.size());

    for (auto &pair : _stats) {
        result.push_back(&pair.second);
    }

    _log.info(_msg("Fetched all deliverers").c_str());
    return result;
}

// std::vector<const ProcessStats *> DelivererService::get_all_stats() {
//     std::vector<const ProcessStats *> result;
//     result.reserve(_deliverers.size());
//
//     for (auto &pair : _deliverers) {
//         result.push_back(pair.second->stats());
//     }
//
//     _log.info(_msg("Fetched all deliverers stats").c_str());
//     return result;
// }

void DelivererService::pause(const std::string &name) {
    auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state != RUNNING) {
        _log.warn(_msg("Deliverer is not running: " + name).c_str());
    }

    _log.info(_msg("Pausing deliverer: " + name).c_str());
    it->second->pause();
}

void DelivererService::resume(const std::string &name) {
    auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state != PAUSED) {
        _log.warn(_msg("Deliverer is not paused: " + name).c_str());
    }

    _log.info(_msg("Resuming deliverer: " + name).c_str());
    it->second->resume();
}

void DelivererService::reload(const std::string &name) {
    auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state != RUNNING) {
        _log.warn(_msg("Deliverer is not running: " + name).c_str());
    }

    _log.info(_msg("Reloading deliverer: " + name).c_str());
    it->second->reload();
}

void DelivererService::reload_all() {
    for (auto &pair : _deliverers) {
        reload(pair.first);
    }
}
