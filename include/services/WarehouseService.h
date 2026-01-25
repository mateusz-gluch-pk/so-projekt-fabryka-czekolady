//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_WAREHOUSESERVICE_H
#define FACTORY_WAREHOUSESERVICE_H
#include <map>
#include <string>

#include "stations/Warehouse.h"

#define SERVICE_DIR "services"

/**
 * @brief Service class for managing Warehouse instances and their statistics.
 */
class WarehouseService {
public:
    /**
     * @brief Constructs a WarehouseService.
     * @param log Reference to a logger.
     * @param debug Whether to enable debug mode (default false).
     * @param child
     */
    explicit WarehouseService(Logger &log, const bool debug = false, const bool child = false):
        _available(make_key(SERVICE_DIR, "warehouses", &log), &log, !child),
        _log(log),
        _debug(debug) {}

    /**
     * @brief Destructor; cleans up all warehouses managed by the service.
     */
    ~WarehouseService();

    /**
     * @brief Creates a new warehouse instance.
     * @param name Name of the warehouse.
     * @param size Item size.
     * @return Pointer to the created Warehouse object.
     * @throws std::runtime_error if creation fails or name already exists.
     */
    IWarehouse *create(const std::string &name, int size);

    /**
     * @brief Creates a new warehouse instance.
     * @param name Name of the warehouse.
     * @param size Item Size.
     * @return Pointer to the created Warehouse object.
     * @throws std::runtime_error if creation fails or name already exists.
     */
    IWarehouse *attach(const std::string &name, int size);

    /**
     * @brief Destroys a warehouse by name.
     * @param name Name of the warehouse to destroy.
     */
    void destroy(const std::string &name);

    /**
     * @brief Retrieves a warehouse by name.
     * @param name Name of the warehouse.
     * @return Pointer to the Warehouse, or nullptr if not found.
     */
    IWarehouse *get(const std::string &name);

    /**
     * @brief Retrieves all warehouses managed by the service.
     * @return Vector of pointers to all Warehouse objects.
     */
    std::vector<IWarehouse *> get_all() const;

    /**
     * @brief Retrieves statistics for a specific warehouse.
     * @param name Name of the warehouse.
     * @return WarehouseStats object for the warehouse.
     * @throws std::runtime_error if warehouse not found.
     */
    WarehouseStats get_stats(const std::string& name);

    /**
     * @brief Retrieves statistics for all warehouses.
     * @return Vector of WarehouseStats objects for all warehouses.
     */
    std::vector<WarehouseStats> get_all_stats() const;

    /// Supervisor function - lock all warehouses
    void lock() {
        if (!_available_sv) {
            _log.warn(_msg("Warehouses already locked").c_str());
            return;
        }
        _log.info(_msg("Locking Warehouses").c_str());
        _available_sv = false;
        _available.lock();
    }

    /// Supervisor function - unlock all warehouses
    void unlock() {
        if (_available_sv) {
            _log.warn(_msg("Warehouses already unlocked").c_str());
            return;
        }
        _log.info(_msg("Unlocking Warehouses").c_str());
        _available_sv = true;
        _available.unlock();
    }

private:
    /**
     * @brief Formats a service log message.
     * @param msg Message content.
     * @return Formatted string with service prefix.
     */
    static std::string _msg(const std::string &msg) {
        return "services/WarehouseService:\t" + msg;
    }

    std::map<std::string, IWarehouse *> _warehouses; ///< Managed warehouse instances

    /// Supervisor lock for warehouses
    Semaphore _available;
    /// bool value to prevent double locking and sem interference
    bool _available_sv = true;

    Logger &_log;                                   ///< Reference to logger
    bool _debug;                                    ///< Debug mode flag
};

#endif //FACTORY_WAREHOUSESERVICE_H