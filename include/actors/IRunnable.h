//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_IRUNNABLE_H
#define PROJEKT_IRUNNABLE_H

#include "processes/ProcessStats.h"
#include <string>
#include <vector>

#include "logger/Logger.h"

/**
 * @brief Interface for a process that can be run, paused, resumed, reloaded, or stopped.
 *
 * Provides a standard interface for use with `ProcessController`. Any class
 * implementing this interface can be managed via IPC-safe process control
 * and signal handling.
 */
class IRunnable {
public:
    /** @brief Virtual destructor for proper cleanup of derived classes. */
    virtual ~IRunnable() = default;

    /**
     * @brief Main execution method of the process.
     * @param stats Reference to ProcessStats to update runtime info.
     * @note Typically invoked in a forked or controlled process context.
     */
    virtual void run(ProcessStats *stats) = 0;

    /**
     * @brief Stop the process (e.g., via SIGTERM).
     */
    virtual void stop() = 0;

    /**
     * @brief Pause the process (e.g., via SIGSTOP).
     */
    virtual void pause() = 0;

    /**
     * @brief Resume a paused process (e.g., via SIGCONT).
     */
    virtual void resume() = 0;

    /**
     * @brief Reload process resources or configuration (e.g., via SIGHUP).
     */
    virtual void reload() = 0;

    /**
     * @brief Get the name of the process.
     * @return Reference to a string containing the process name.
     */
    virtual const std::string &name() = 0;

    virtual std::vector<std::string> argv() = 0;
};



#endif //PROJEKT_IRUNNABLE_H