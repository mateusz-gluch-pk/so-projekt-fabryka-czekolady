//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_WAREHOUSESERVICE_H
#define FACTORY_WAREHOUSESERVICE_H
#include <map>
#include <string>

#include "stations/Warehouse.h"


class WarehouseService {
    public:
    explicit WarehouseService(Logger &log): _log(log) {}

    ~WarehouseService() {
        _log.info(_msg("Clearing warehouses").c_str());
        for (auto &pair: _warehouses) {
            delete pair.second;
        }
        _warehouses.clear();
    }

    Warehouse *create(const std::string &name, int capacity) {
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

    void destroy(const std::string &name) {
        auto it = _warehouses.find(name);
        if (it == _warehouses.end()) {
            _log.error(_msg("Warehouse not found: " + name).c_str());
            return;
        }

        delete it->second;
        _warehouses.erase(it);
        _log.info(_msg("Deleted warehouse: " + name).c_str());
    }

    Warehouse *get(const std::string &name) {
        auto it = _warehouses.find(name);
        if (it == _warehouses.end()) {
            _log.error(_msg("Warehouse not found: " + name).c_str());
            return nullptr;
        }

        _log.info(_msg("Fetched warehouse: " + name).c_str());
        return it->second;
    }

    WarehouseStats get_stats(const std::string& name) {
        auto it = _warehouses.find(name);
        if (it == _warehouses.end()) {
            _log.error(_msg("Warehouse not found: " + name).c_str());
            return {name, 0, 0, 0, std::vector<Item>()};
        }

        const auto *wh = it->second;
        _log.info(_msg("Fetched warehouse stats: " + name).c_str());
        return {name, wh->capacity(), wh->variety(), wh->usage(), wh->items()};
    };

    std::vector<Warehouse *> get_all() {
        std::vector<Warehouse *> result;
        result.reserve(_warehouses.size());

        for (auto &pair : _warehouses) {
            result.push_back(pair.second);
        }

        _log.info(_msg("Fetched all warehouses").c_str());
        return result;
    }

    std::vector<WarehouseStats> get_all_stats() {
        std::vector<WarehouseStats> result;
        result.reserve(_warehouses.size());

        for (auto &pair : _warehouses) {
            const auto *wh = pair.second;
            result.emplace_back(wh->name(), wh->capacity(), wh->variety(), wh->usage(), wh->items());
        }

        _log.info(_msg("Fetched all warehouses stats").c_str());
        return result;
    };

    private:
    static std::string _msg(const std::string &msg) {
        return "services/WarehouseService:\t" + msg;
    }

    std::map<std:: string, Warehouse *> _warehouses;
    Logger &_log;
};


#endif //FACTORY_WAREHOUSESERVICE_H