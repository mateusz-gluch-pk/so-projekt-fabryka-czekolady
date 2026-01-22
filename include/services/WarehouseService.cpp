//
// Created by mateusz on 27.12.2025.
//

#include "WarehouseService.h"

#define CAPACITY 1024

WarehouseService::~WarehouseService() {
    _log.info(_msg("Clearing warehouses").c_str());
    for (const auto &pair: _warehouses) {
        delete pair.second;
    }
    _warehouses.clear();
}

IWarehouse * WarehouseService::attach(const std::string &name, int size) {
    if (_warehouses.contains(name)) {
        _log.error(_msg("Warehouse exists: " + name).c_str());
        return nullptr;
    }

    try {
        IWarehouse *wh = nullptr;
        switch (size) {
            case 1:
                wh = new Warehouse<1, CAPACITY>(name, &_log, false);
                break;
            case 2:
                wh = new Warehouse<2, CAPACITY>(name, &_log, false);
                break;
            case 3:
                wh = new Warehouse<3, CAPACITY>(name, &_log, false);
                break;
            default:
                throw std::invalid_argument(_msg("Warehouse size not supported").c_str());
        }
        _warehouses[name] = wh;
        _log.info(_msg("Created warehouse: " + name).c_str());
        return wh;
    } catch (...) {
        _log.fatal(_msg("Failed to create warehouse: " + name).c_str());
        return nullptr;
    }
}


IWarehouse * WarehouseService::create(const std::string &name, int size) {
    if (_warehouses.contains(name)) {
        _log.error(_msg("Warehouse exists: " + name).c_str());
        return nullptr;
    }

    try {
        IWarehouse *wh = nullptr;
        switch (size) {
            case 1:
                wh = new Warehouse<1, CAPACITY>(name, &_log);
                break;
            case 2:
                wh = new Warehouse<2, CAPACITY>(name, &_log);
                break;
            case 3:
                wh = new Warehouse<3, CAPACITY>(name, &_log);
                break;
            default:
                throw std::invalid_argument(_msg("Warehouse size not supported").c_str());
        }
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

IWarehouse * WarehouseService::get(const std::string &name) {
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
        return {name, 0, 0, 0, 0, 0};
    }

    const auto *wh = it->second;
    _log.debug(_msg("Fetched warehouse stats: " + name).c_str());
    return {name, wh->size(), wh->capacity(), wh->usage(), wh->empty(), wh->full()};
}

std::vector<IWarehouse *> WarehouseService::get_all() const {
    std::vector<IWarehouse *> result;
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
        result.emplace_back(wh->name(), wh->size(), wh->capacity(), wh->usage(), wh->empty(), wh->full());
    }

    _log.debug(_msg("Fetched all warehouses stats").c_str());
    return result;
}
