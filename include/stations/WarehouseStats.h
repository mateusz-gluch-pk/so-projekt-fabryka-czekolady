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
     * @param size Size of item in the warehouse.
     * @param capacity Maximum number of items the warehouse can hold.
     * @param usage Current number of items stored.
     * @param empty
     * @param full
     */
    WarehouseStats(std::string name, int size, int capacity, int usage, int empty, int full);

    /** @brief Get the warehouse name. */
    [[nodiscard]] const std::string &name() const { return _name; }

    /** @brief Get warehouse capacity. */
    [[nodiscard]] int capacity() const { return _capacity; }

    /** @brief Get warehouse item size. */
    [[nodiscard]] int size() const { return _size; }

    /** @brief Get current number of items stored. */
    [[nodiscard]] int usage() const { return _usage; }

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
    int _capacity;                /**< Maximum number of items the warehouse can hold */
    int _size, _empty, _full;
    int _usage;                   /**< Current number of items stored */
};

inline WarehouseStats::WarehouseStats(std::string name, const int size, const int capacity, const int usage, const int empty, const int full):
    _name(std::move(name)), _capacity(capacity), _size(size), _usage(usage), _empty(empty), _full(full) {}

inline std::vector<std::string> WarehouseStats::row() const {
    return std::vector<std::string>{
        _name,
        std::to_string(_size),
        std::to_string(_usage),
        std::to_string(_capacity),
        std::to_string(_empty),
        std::to_string(_full)
    };
}

inline std::vector<std::string> WarehouseStats::headers() {
    return std::vector<std::string>{
        "Name",
        "Item Size",
        "Usage",
        "Capacity",
        "Empty",
        "Full"
    };
}


#endif //FACTORY_WAREHOUSESTATS_H
