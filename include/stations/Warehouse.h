//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <filesystem>
#include <vector>
#include <sys/types.h>

#include "WarehouseStats.h"
#include "objects/SharedVector.h"
#include "ipcs/Semaphore.h"
#include "ipcs/SharedMemory.h"
#include "objects/Item.h"

#define WAREHOUSE_MAX_VARIETY 1024

namespace fs = std::filesystem;

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
class Warehouse {
public:
    /**
     * @brief Attach to an existing warehouse.
     * @param name Warehouse name.
     * @param capacity Maximum capacity.
     * @param log Logger for debug/info/error messages.
     * @return Warehouse instance attached to existing IPC resources.
     */
    static Warehouse attach(const std::string& name, int capacity, Logger *log);

    /**
     * @brief Create a new warehouse.
     * @param name Warehouse name.
     * @param capacity Maximum capacity.
     * @param log Logger for debug/info/error messages.
     * @return Warehouse instance owning the IPC resources.
     */
    static Warehouse create(const std::string& name, int capacity, Logger *log);

    /**
     * @brief Construct a warehouse (attach or create).
     * @param name Warehouse name.
     * @param capacity Maximum capacity.
     * @param log Logger instance.
     * @param create True to create new resources; false to attach existing.
     */
    Warehouse(const std::string& name, int capacity, Logger *log, bool create = true);

    /** @brief Destructor cleans up IPC resources if owned. */
    ~Warehouse();

    /**
     * @brief Add an item to the warehouse.
     * @param item Item to add.
     * @note Thread/process-safe via internal semaphore.
     */
    void add(Item &item) const;

    /**
     * @brief Retrieve an item from the warehouse by name.
     * @param itemName Name of the item to fetch.
     * @param output Pointer to store the retrieved item; may be null.
     */
    void get(const std::string &itemName, Item *output) const;

    /** @brief Get a copy of all items currently in the warehouse. */
    [[nodiscard]] std::vector<Item> items() const;

    /** @brief Get the warehouse name. */
    [[nodiscard]] std::string name() const;

    /** @brief Get the maximum capacity of the warehouse. */
    [[nodiscard]] int capacity() const;

    /** @brief Get number of distinct item types (variety) in the warehouse. */
    static int variety();

    /** @brief Get current number of items stored (usage). */
    [[nodiscard]] int usage() const;

private:
    /**
     * @brief Internal constructor used by `attach`/`create`.
     * @param name Warehouse name.
     * @param capacity Maximum capacity.
     * @param total_size Total shared memory size in bytes.
     * @param key IPC key for shared memory and semaphore.
     * @param log Logger instance.
     * @param create True to create resources; false to attach.
     */
    Warehouse(std::string name, int capacity, size_t total_size, key_t key, Logger *log, bool create);

    /**
     * @brief Format a log message with warehouse context.
     * @param msg Message text.
     * @return Formatted string.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const {
        return "stations/Warehouse/" + _name + ":\t" + msg;
    }

    int _capacity;  /**< Maximum number of items warehouse can hold */
    std::string _name;  /**< Warehouse name */
    fs::path _filename; /**< Path for file-based persistence */

    // IPC resources
    bool _owner;  /**< True if this instance created the IPC resources */
    Semaphore _sem;  /**< Semaphore for thread/process-safe access */
    SharedMemory<SharedVector<Item, WAREHOUSE_MAX_VARIETY>> _shm; /**< Shared memory for items */
    SharedVector<Item, WAREHOUSE_MAX_VARIETY> *_content; /**< Pointer to shared memory content */

    // Logger
    Logger *_log;  /**< Logger instance for debug/info/error messages */

    // File persistence helpers
    void _write_file();
    void _read_file() const;
};

#endif //PROJEKT_WAREHOUSE_H