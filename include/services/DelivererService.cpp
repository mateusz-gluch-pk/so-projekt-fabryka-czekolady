//
// Created by mateusz on 27.12.2025.
//

#include "DelivererService.h"

std::vector<std::string> DelivererStats::headers() {
    return std::vector<std::string>{
        "Name",
        "Item",
        "Base Delay [ms]",
        "Status",
        "Loops",
        "Reloads",
        "PID"
    };
}

std::vector<std::string> DelivererStats::row() const {
    return std::vector{
        name,
        tpl.get()->name(),
        std::to_string(tpl.base_delay_ms()),
        state_to_string(stats->state),
        std::to_string(stats->loops),
        std::to_string(stats->reloads),
        std::to_string(stats->pid),
    };
}

DelivererService::~DelivererService() {
    for (auto &pair: _deliverers) {
        if (pair.second->stats()->state != STOPPED) {
            pair.second->stop();
        }
        delete pair.second;
    }

    _deliverers.clear();
    _stats.clear();
}

DelivererStats *DelivererService::create(const std::string &name, const ItemTemplate &tpl, WarehouseService &svc) {
    if (_deliverers.contains(name)) {
        _log.error(_msg("Deliverer exists: " + name).c_str());
        return nullptr;
    }

    try {
        auto d = std::make_unique<Deliverer>(name, tpl, svc, _log);
        const auto pd = new ProcessController(std::move(d), _log, true, _debug);
        pd->run();
        _deliverers[name] = pd;
        _stats[name] = DelivererStats(name, tpl, pd->stats());
        _log.info(_msg("Created deliverer: " + name).c_str());
        return &_stats[name];
    } catch (...) {
        _log.error(_msg("Failed to create deliverer: " + name).c_str());
        return nullptr;
    }
}

void DelivererService::destroy(const std::string &name) {
    auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
        return;
    }

    it->second->stop();
    _log.info(_msg("(Soft) Deleted deliverer: " + name).c_str());
}

DelivererStats *DelivererService::get(const std::string &name) {
    auto it = _stats.find(name);
    if (it == _stats.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
        return nullptr;
    }

    _log.debug(_msg("Fetched deliverer: " + name).c_str());
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

    _log.debug(_msg("Fetched all deliverers").c_str());
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
    const auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state == STOPPED) {
        _log.warn(_msg("Deliverer is terminated: " + name).c_str());
        return;
    }
    if (state != RUNNING) {
        _log.warn(_msg("Deliverer is not running: " + name).c_str());
    }

    _log.info(_msg("Pausing deliverer: " + name).c_str());
    it->second->pause();
    while (it->second->stats()->state != PAUSED) {
        sthr::sleep_for(std::chrono::milliseconds(10));
    }
}

void DelivererService::resume(const std::string &name) {
    const auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state == STOPPED) {
        _log.warn(_msg("Deliverer is terminated: " + name).c_str());
        return;
    }
    if (state != PAUSED) {
        _log.warn(_msg("Deliverer is not paused: " + name).c_str());
    }

    _log.info(_msg("Resuming deliverer: " + name).c_str());
    it->second->resume();
}

void DelivererService::reload(const std::string &name) {
    const auto it = _deliverers.find(name);
    if (it == _deliverers.end()) {
        _log.error(_msg("Deliverer not found: " + name).c_str());
    }

    const ProcessState state = it->second->stats()->state;
    if (state == STOPPED) {
        _log.warn(_msg("Deliverer is terminated: " + name).c_str());
        return;
    }
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

std::string DelivererService::_msg(const std::string &msg) {
    return "services/DeliverersService:\t" + msg;
}
