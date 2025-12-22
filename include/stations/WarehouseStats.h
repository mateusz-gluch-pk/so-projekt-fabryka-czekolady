//
// Created by mateusz on 22.12.2025.
//

#ifndef FACTORY_WAREHOUSESTATS_H
#define FACTORY_WAREHOUSESTATS_H
#include <utility>
#include <vector>

#include "objects/SharedVector.h"


class WarehouseStats {
    public:
    WarehouseStats(std::string name, const int capacity, const int variety, const int usage):
    _name(std::move(name)), _capacity(capacity), _variety(variety), _usage(usage) {}

    // [[nodiscard]] const std::vector<Item>& items() const { return _items; }
    [[nodiscard]] const std::string &name() const {return _name;}
    [[nodiscard]] int capacity() const {return _capacity;}
    [[nodiscard]] int variety() const {return _variety;}
    [[nodiscard]] int usage() const {return _usage;}

    private:
    std::string _name;
    // std::vector<Item> _items;
    int _capacity;
    int _variety;
    int _usage;
};


#endif //FACTORY_WAREHOUSESTATS_H