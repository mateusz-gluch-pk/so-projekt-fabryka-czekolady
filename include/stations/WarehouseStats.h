//
// Created by mateusz on 22.12.2025.
//

#ifndef FACTORY_WAREHOUSESTATS_H
#define FACTORY_WAREHOUSESTATS_H
#include <utility>
#include <vector>

#include "objects/SharedVector.h"


/**
 * @brief Holds statistical information about a warehouse.
 *
 * Stores warehouse metadata such as name, capacity, number of item types (variety),
 * current usage, and a list of stored items.
 */
class WarehouseStats {
public:
    /**
     * @brief Construct a WarehouseStats object.
     * @param name Name of the warehouse.
     * @param capacity Maximum number of items the warehouse can hold.
     * @param variety Number of distinct item types.
     * @param usage Current number of items stored.
     * @param items List of items stored in the warehouse.
     */
    WarehouseStats(std::string name, int capacity, int variety, int usage, const std::vector<Item> &items);

    /** @brief Get the warehouse name. */
    [[nodiscard]] const std::string &name() const { return _name; }

    /** @brief Get warehouse capacity. */
    [[nodiscard]] int capacity() const { return _capacity; }

    /** @brief Get number of distinct item types in the warehouse. */
    [[nodiscard]] int variety() const { return _variety; }

    /** @brief Get current number of items stored. */
    [[nodiscard]] int usage() const { return _usage; }

    /** @brief Get a copy of the items stored in the warehouse. */
    [[nodiscard]] std::vector<Item> items() const { return _items; }

    /**
     * @brief Get warehouse stats as a vector of strings (one row for tabular display).
     * @return Vector of string representations of the stats.
     */
    [[nodiscard]] std::vector<std::string> row() const;

    /**
     * @brief Get table headers for warehouse statistics.
     * @return Vector of strings representing column headers.
     */
    static std::vector<std::string> headers();

private:
    std::string _name;            /**< Warehouse name */
    std::vector<Item> _items;     /**< List of items in the warehouse */
    int _capacity;                /**< Maximum number of items the warehouse can hold */
    int _variety;                 /**< Number of distinct item types */
    int _usage;                   /**< Current number of items stored */
};

inline WarehouseStats::WarehouseStats(std::string name, const int capacity, const int variety, const int usage,
    const std::vector<Item> &items):
    _name(std::move(name)), _items(items), _capacity(capacity), _variety(variety), _usage(usage) {}

inline std::vector<std::string> WarehouseStats::row() const {
    return std::vector<std::string>{
        _name,
        std::to_string(_items.size()),
        std::to_string(_capacity),
        std::to_string(_variety),
        std::to_string(_usage),
    };
}

inline std::vector<std::string> WarehouseStats::headers() {
    return std::vector<std::string>{
        "Name",
        "Item Count",
        "Item Capacity",
        "Item Variety",
        "Usage",
    };
}


#endif //FACTORY_WAREHOUSESTATS_H
