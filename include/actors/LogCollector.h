//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_LOGCOLLECTOR_H
#define PROJEKT_LOGCOLLECTOR_H
#include <atomic>
#include <fstream>
#include <optional>
#include <thread>

#include "IRunnable.h"
#include "ipcs/key.h"
#include "objects/Message.h"
#include "ipcs/MessageQueue.h"
#include "ipcs/SharedMemory.h"
#include "objects/SharedVector.h"

#define LOGGING_DIR "logging"
#define LOGGING_BUFFER_SIZE 1024

/**
 * @brief Collects and manages log messages in a separate thread.
 */
class LogCollector: public IRunnable {
public:
    /**
     * @brief Constructs a LogCollector.
     * @param name Name of the log collector.
     * @param log Reference to a logger.
     * @param tty Whether output should be TTY-formatted (default true).
     * @throws std::exception if initialization fails.
     */
    explicit LogCollector(std::string name, Logger &log, bool tty = true);

    /**
     * @brief Destructor; closes file and cleans up resources.
     */
    ~LogCollector() override;

    /**
     * @brief Main execution loop for collecting logs.
     * @param stats Reference to process statistics.
     * @param log Reference to a logger for runtime messages.
     * @throws std::runtime_error on logging failure.
     */
    void run(ProcessStats *stats) override;

    /**
     * @brief Stops log collection safely.
     */
    void stop() override;

    /**
     * @brief Pauses log collection.
     */
    void pause() override;

    /**
     * @brief Resumes log collection after pause.
     */
    void resume() override;

    /**
     * @brief Reloads internal state or configuration.
     */
    void reload() override;

    /**
     * @brief Returns the name of the log collector.
     * @return Reference to the collector name.
     */
    const std::string &name() override { return _name; }

    // Args to run exec
    std::vector<std::string> argv() override;

private:
    /**
     * @brief Internal main loop for log collection.
     */
    void _main();

    /**
     * @brief Reloads internal resources (no-op placeholder).
     */
    void _reload() {};

    /**
     * @brief Reattaches the collector to a new logger and resets resources.
     * @param log Reference to a new logger.
     */
    void _reattach(Logger &log);

    /**
     * @brief Generates a formatted log message.
     * @param msg Message content.
     * @return Formatted string with collector prefix.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const;

    /**
     * @brief Opens the log file for writing.
     * @return Output file stream object.
     * @throws std::runtime_error if file cannot be opened.
     */
    std::ofstream _open_file() const;

    /**
     * @brief Closes the currently opened log file.
     */
    void _close_file();

    std::string _name;                       ///< Name of the log collector
    bool _tty;                               ///< TTY output flag
    std::optional<MessageQueue<Message>> _msq; ///< Optional message queue
    std::optional<SharedMemory<SharedVector<Message, LOGGING_BUFFER_SIZE>>> _buffer; ///< Optional shared memory buffer
    Logger &_log;                             ///< Reference to logger
    std::ofstream _file;                      ///< Output file stream
    std::atomic<bool> _running, _paused, _reloading; ///< Thread control flags
};

#endif //PROJEKT_LOGCOLLECTOR_H