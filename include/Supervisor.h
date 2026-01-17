//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SUPERVISOR_H
#define PROJEKT_SUPERVISOR_H

#include "services/DelivererService.h"
#include "services/LoggerService.h"
#include "services/WarehouseService.h"
#include "services/WorkerService.h"

#include <atomic>

/**
 * @brief Thread-safe token used to signal shutdown requests.
 */
class ShutdownToken {
public:
    /**
     * @brief Requests shutdown.
     */
    void request() { stop.store(true); }

    /**
     * @brief Checks whether shutdown was requested.
     * @return True if shutdown was requested, false otherwise.
     */
    bool requested() const { return stop.load(); }

private:
    std::atomic<bool> stop{false}; ///< Shutdown request flag
};

/**
 * @brief Coordinates orderly shutdown of system services.
 */
class Supervisor {
public:
    /**
     * @brief Constructs a Supervisor.
     * @param warehouses Reference to warehouse service.
     * @param deliverers Reference to deliverer service.
     * @param workers Reference to worker service.
     * @param exit Reference to shutdown token.
     * @param log Reference to logger.
     */
    explicit Supervisor(
        WarehouseService &warehouses,
        DelivererService &deliverers,
        WorkerService &workers,
        ShutdownToken &exit,
        Logger &log
    );

    /**
     * @brief Default destructor.
     */
    ~Supervisor() = default;

    /**
     * @brief Stops all deliverers.
     */
    void stop_deliverers() const;

    /**
     * @brief Stops all workers.
     */
    void stop_workers() const;

    /**
     * @brief Stops all warehouses.
     */
    void stop_warehouses() const;

    /**
     * @brief Stops all managed services.
     */
    void stop_all() const;

private:
    Logger &_log;                     ///< Reference to logger
    ShutdownToken &_exit;             ///< Shutdown coordination token
    WarehouseService &_warehouses;    ///< Managed warehouse service
    WorkerService &_workers;          ///< Managed worker service
    DelivererService &_deliverers;    ///< Managed deliverer service
};

#endif //PROJEKT_SUPERVISOR_H