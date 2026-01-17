//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKER_H
#define PROJEKT_WORKER_H
#include <atomic>
#include <bits/this_thread_sleep.h>

#include "IRunnable.h"
#include "ipcs/MessageQueue.h"
#include "stations/Warehouse.h"
#include "stations/Workstation.h"
#include "logger/Logger.h"
#include "objects/Recipe.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;

#define WORKER_TICK_DELAY 1000

/**
 * @brief Represents a worker that processes recipes between warehouses.
 */
class Worker : public IRunnable {
public:
    /**
     * @brief Constructs a Worker.
     * @param name Name of the worker.
     * @param recipe Recipe to process.
     * @param in Reference to input warehouse.
     * @param out Reference to output warehouse.
     * @param log Reference to logger.
     * @throws std::exception if warehouse or logger setup fails.
     */
    Worker(std::string name, Recipe recipe, Warehouse &in, Warehouse &out, Logger &log);

    /**
     * @brief Main execution loop for processing items.
     * @param stats Reference to process statistics to update.
     * @param log Reference to a logger for runtime messages.
     * @throws std::runtime_error on processing failure.
     */
    void run(ProcessStats &stats, Logger &log) override;

    /**
     * @brief Stops the worker safely.
     */
    void stop() override;

    /**
     * @brief Pauses the worker's execution.
     */
    void pause() override;

    /**
     * @brief Resumes execution after pause.
     */
    void resume() override;

    /**
     * @brief Reloads internal state or configuration.
     */
    void reload() override;

    /**
     * @brief Returns the name of the worker.
     * @return Reference to the worker's name string.
     */
    const std::string &name() override { return _name; }

private:
    /**
     * @brief Internal main loop for processing items.
     */
    void _main();

    /**
     * @brief Internal reload implementation.
     */
    void _reload();

    /**
     * @brief Reattaches the worker to a new logger and resets warehouses.
     * @param log Reference to a new logger.
     */
    void _reattach(Logger &log) {
        _log = log;
        _in.emplace(_in->name(), _in->capacity(), &log, false);
        _out.emplace(_out->name(), _out->capacity(), &log, false);
    }

    /**
     * @brief Generates a formatted log message.
     * @param msg Message content.
     * @return Formatted string with worker prefix.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const;

    std::string _name;                 ///< Name of the worker
    Recipe _recipe;                     ///< Recipe to process
    std::optional<Warehouse> _in;       ///< Optional input warehouse
    std::optional<Warehouse> _out;      ///< Optional output warehouse
    Logger &_log;                        ///< Reference to logger
    std::vector<Item> _inventory;       ///< Worker inventory
    std::atomic<bool> _running, _paused, _reloading; ///< Thread control flags
};

#endif //PROJEKT_WORKER_H