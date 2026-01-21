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
#include "services/WarehouseService.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;

#define WORKER_TICK_DELAY 1000
// #define WORKER_TICK_DELAY 0

/**
 * @brief Represents a worker that processes recipes between warehouses.
 */
class Worker : public IRunnable {
public:
    /**
     * @brief Constructs a Worker.
     * @param name Name of the worker.
     * @param recipe Recipe to process.
     * @param svc Warehouse service (for parent or child)
     * @param log Reference to logger.
     * @param child indicator if this is a child process
     * @throws std::exception if warehouse or logger setup fails.
     */
    Worker(std::string name, std::unique_ptr<Recipe> recipe, WarehouseService &svc, Logger &log, bool child = false);

    /**
     * @brief Main execution loop for processing items.
     * @param stats Reference to process statistics to update.
     * @throws std::runtime_error on processing failure.
     */
    void run(ProcessStats *stats) override;

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
        std::vector<std::string> names;
        std::vector<int> sizes;
        for (auto &wh : _svc.get_all_stats()) {
            names.push_back(wh.name());
            sizes.push_back(wh.size());
        }
        for (int i = 0; i < names.size(); i++) {
            _svc.destroy(names[i]);
            auto wh = _svc.attach(names[i], sizes[i]);
            if (wh == nullptr) {
                _log.error(_msg("Cannot reattach to warehouse Item<%d>(\"%s\")").c_str(), names[i].c_str(), sizes[i]);
            }
        }
    }

    /**
     * @brief Generates a formatted log message.
     * @param msg Message content.
     * @return Formatted string with worker prefix.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const;

    // Args to run exec
    std::vector<std::string> argv() override;

    std::string _name;                 ///< Name of the worker
    std::unique_ptr<Recipe> _recipe;                     ///< Recipe to process
    WarehouseService &_svc;               ///<Service to access warehouses
    Logger &_log;                        ///< Reference to logger
    std::vector<std::unique_ptr<IItem>> _inventory;       ///< Worker inventory
    std::atomic<bool> _running, _paused, _reloading; ///< Thread control flags
};

#endif //PROJEKT_WORKER_H