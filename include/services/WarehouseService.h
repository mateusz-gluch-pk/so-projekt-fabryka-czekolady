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
    ~WarehouseService();

    Warehouse *create(const std::string &name, int capacity);
    void destroy(const std::string &name);

    Warehouse *get(const std::string &name);
    std::vector<Warehouse *> get_all();

    WarehouseStats get_stats(const std::string& name);
    std::vector<WarehouseStats> get_all_stats();

private:
    static std::string _msg(const std::string &msg) {
        return "services/WarehouseService:\t" + msg;
    }

    std::map<std:: string, Warehouse *> _warehouses;
    Logger &_log;
};

#endif //FACTORY_WAREHOUSESERVICE_H