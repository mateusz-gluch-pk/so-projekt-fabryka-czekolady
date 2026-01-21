//
// Created by mateusz on 20.01.2026.
//

#ifndef FACTORY_IWAREHOUSE_H
#define FACTORY_IWAREHOUSE_H

#include <string>
#include <vector>

#include "objects/IItem.h"

/**
 * @brief Represents a warehouse with IPC-safe storage and concurrency control.
 *
 * Provides an interface to add and retrieve items while supporting multiple
 * processes via System V shared memory and semaphores. Supports RAII-style
 * creation and automatic cleanup.
 *
 * Ownership model:
 * - If created via `create`, the instance owns the underlying IPC resources
 *   (shared memory, semaphore) and deletes them in the destructor.
 * - If attached via `attach`, the instance does not own the resources and
 *   leaves them intact on destruction.
 */
class IWarehouse {
public:
    virtual ~IWarehouse() = default;  // REQUIRED

    // Core operations
    /**
     * @brief Add an item to the warehouse.
     * @param item Item to add.
     * @note Thread/process-safe via internal semaphore.
    */
    virtual void add(IItem& item) const = 0;

    /**
     * @brief Retrieve an item from the warehouse by name.
     * @param itemName Name of the item to fetch.
     * @return output Pointer to store the retrieved item; may be null.
     */
    virtual std::unique_ptr<IItem> get(const std::string& itemName) const = 0;

    // Queries
    /** @brief Get the warehouse name. */
    [[nodiscard]] virtual std::string name() const = 0;

    /** @brief Get the maximum capacity of the warehouse. */
    [[nodiscard]] virtual int capacity() const = 0;

    /** @brief Get current number of items stored (usage). */
    [[nodiscard]] virtual int usage() const = 0;
};

#endif //FACTORY_IWAREHOUSE_H