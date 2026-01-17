//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_PROCESSCONTROLLER_H
#define PROJEKT_PROCESSCONTROLLER_H
#include <csignal>
#include <memory>
#include <optional>

#include "ProcessStats.h"
#include "actors/IRunnable.h"
#include "ipcs/MessageQueue.h"
#include "ipcs/SharedMemory.h"

/**
 * @brief Manages the lifecycle of a process with IPC-safe statistics and control.
 *
 * Provides a RAII-style interface to start, pause, resume, reload, and stop a process.
 * Tracks runtime statistics via shared memory, allowing external observers to query
 * process state safely. Signal handlers are used for asynchronous control.
 *
 * Ownership model:
 * - The `ProcessController` owns the `IRunnable` instance and manages its lifecycle.
 * - Shared memory for `ProcessStats` is owned by the controller instance.
 * - Optional message queue and logger allow for inter-process communication.
 */
class ProcessController {
public:
    /**
     * @brief Construct a process controller.
     * @param proc Unique pointer to an IRunnable process implementation.
     * @param log Logger for debug/info/error messages.
     * @param create True to create IPC resources; false to attach existing.
     * @param debug Enable debug logging if true.
     */
    explicit ProcessController(std::unique_ptr<IRunnable> proc, const Logger &log, bool create = true, bool debug = false);

    /** @brief Destructor stops the process and cleans up IPC resources. */
    ~ProcessController();

    /** @brief Start or resume the process execution. */
    void run();

    /** @brief Stop the process and release resources. */
    void stop();

    /** @brief Pause the process execution (non-blocking). */
    void pause() const;

    /** @brief Resume a paused process. */
    void resume() const;

    /** @brief Reload the process (e.g., configuration or resources). */
    void reload() const;

    /** @brief Get a pointer to the current process statistics. */
    [[nodiscard]] const ProcessStats *stats() const { return _stats.get(); }

private:
    /** @brief Setup signal handlers for process control. */
    static void _setup_handlers();

    /** @brief Signal handler for stop requests. */
    static void _handle_stop(int);

    /** @brief Signal handler for pause requests. */
    static void _handle_pause(int);

    /** @brief Signal handler for resume requests. */
    static void _handle_resume(int);

    /** @brief Signal handler for reload requests. */
    static void _handle_reload(int);

    /**
     * @brief Format a log message with process context.
     * @param msg Message text.
     * @return Formatted string.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const {
        std::ostringstream ss;
        ss << _pid;
        return "processes/ProcessController/" + ss.str() + ":\t" + msg;
    }

    bool _debug = false; /**< Enable debug logging */
    key_t _key;          /**< IPC key for message queue / shared memory */
    MessageQueue<Message> *_msq = nullptr; /**< Optional message queue */
    Logger _log;         /**< Logger instance */

    std::unique_ptr<IRunnable> _proc; /**< Owned process implementation */
    SharedMemory<ProcessStats> _stats; /**< Shared memory for process statistics */
    static std::unique_ptr<ProcessController> _cls; /**< Singleton instance for signal handling */

    pid_t _pid; /**< Process ID */
};


#endif //PROJEKT_PROCESSCONTROLLER_H