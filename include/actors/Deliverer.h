//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_DELIVERER_H
#define PROJEKT_DELIVERER_H
#include <atomic>
#include <thread>

#include "IRunnable.h"
#include "ipcs/MessageQueue.h"
#include "objects/ItemTemplate.h"
#include "stations/Warehouse.h"
#include "logger/Logger.h"
#include "processes/ProcessStats.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;


/**
 * @brief Represents a delivery worker that runs tasks and interacts with a warehouse.
 */
class Deliverer : public IRunnable {
public:
    /**
     * @brief Constructs a Deliverer.
     * @param name Name of the deliverer.
     * @param tpl Template of the item to deliver.
     * @param dst Target warehouse reference.
     * @param log Logger reference.
     * @throws std::exception if warehouse or logger setup fails.
     */
    Deliverer(std::string name, ItemTemplate tpl, Warehouse &dst, Logger &log);

    /**
     * @brief Main execution function for the deliverer.
     * @param stats Reference to process statistics to update.
     * @param log Reference to a logger for runtime messages.
     * @throws std::runtime_error on delivery failure.
     */
    void run(ProcessStats &stats, Logger &log) override;

    /**
     * @brief Stops the deliverer thread safely.
     */
    void stop() override;

    /**
     * @brief Pauses the deliverer's execution.
     */
    void pause() override;

    /**
     * @brief Resumes execution after a pause.
     */
    void resume() override;

    /**
     * @brief Reloads internal state or configuration.
     */
    void reload() override;

    /**
     * @brief Returns the deliverer's name.
     * @return Reference to the name string.
     */
    const std::string &name() override { return _name; }

private:
    /**
     * @brief Internal main loop for delivery logic.
     */
    void _main() const;

    /**
     * @brief Internal reload implementation.
     */
    void _reload();

    /**
     * @brief Reattaches the deliverer to a new logger and warehouse.
     * @param log Logger reference.
     */
    void _reattach(Logger &log);

    /**
     * @brief Generates a formatted log message.
     * @param msg Message content.
     * @return Formatted string with deliverer prefix.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const;

    std::string _name;                        ///< Name of the deliverer
    ItemTemplate _tpl;                        ///< Item template to deliver
    std::optional<Warehouse> _dst;            ///< Optional target warehouse
    Logger &_log;                              ///< Reference to logger
    std::atomic<bool> _running, _paused, _reloading; ///< Thread control flags
};


#endif //PROJEKT_DELIVERER_H