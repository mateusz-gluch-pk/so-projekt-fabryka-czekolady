//
// Created by mateusz on 27.12.2025.
//

#include "WarehouseService.h"

WarehouseService::~WarehouseService() {
    _log.info(_msg("Clearing warehouses").c_str());
    for (const auto &pair: _warehouses) {
        delete pair.second;
    }
    _warehouses.clear();
}

Warehouse * WarehouseService::create(const std::string &name, int capacity) {
    if (_warehouses.contains(name)) {
        _log.error(_msg("Warehouse exists: " + name).c_str());
        return nullptr;
    }

    try {
        auto *wh = new Warehouse(name, capacity, &_log);
        _warehouses[name] = wh;
        _log.info(_msg("Created warehouse: " + name).c_str());
        return wh;
    } catch (...) {
        _log.error(_msg("Failed to create warehouse: " + name).c_str());
        return nullptr;
    }
}

void WarehouseService::destroy(const std::string &name) {
    const auto it = _warehouses.find(name);
    if (it == _warehouses.end()) {
        _log.error(_msg("Warehouse not found: " + name).c_str());
        return;
    }

    delete it->second;
    _warehouses.erase(it);
    _log.info(_msg("Deleted warehouse: " + name).c_str());
}

Warehouse * WarehouseService::get(const std::string &name) {
    const auto it = _warehouses.find(name);
    if (it == _warehouses.end()) {
        _log.error(_msg("Warehouse not found: " + name).c_str());
        return nullptr;
    }

    _log.debug(_msg("Fetched warehouse: " + name).c_str());
    return it->second;
}

WarehouseStats WarehouseService::get_stats(const std::string &name) {
    const auto it = _warehouses.find(name);
    if (it == _warehouses.end()) {
        _log.error(_msg("Warehouse not found: " + name).c_str());
        return {name, 0, 0, 0, std::vector<Item>()};
    }

    const auto *wh = it->second;
    _log.debug(_msg("Fetched warehouse stats: " + name).c_str());
    return {name, wh->capacity(), wh->variety(), wh->usage(), wh->items()};
}

std::vector<Warehouse *> WarehouseService::get_all() const {
    std::vector<Warehouse *> result;
    result.reserve(_warehouses.size());

    for (auto &pair : _warehouses) {
        result.push_back(pair.second);
    }

    _log.debug(_msg("Fetched all warehouses").c_str());
    return result;
}

std::vector<WarehouseStats> WarehouseService::get_all_stats() const {
    std::vector<WarehouseStats> result;
    result.reserve(_warehouses.size());

    for (auto &pair : _warehouses) {
        const auto *wh = pair.second;
        result.emplace_back(wh->name(), wh->capacity(), wh->variety(), wh->usage(), wh->items());
    }

    _log.debug(_msg("Fetched all warehouses stats").c_str());
    return result;
}
