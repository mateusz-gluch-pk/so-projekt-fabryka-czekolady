//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_LOGGER_H
#define PROJEKT_LOGGER_H
#include <stdarg.h>

#include "IQueue.h"
#include "objects/Message.h"

/**
 * @brief Logger for messages with different severity levels.
 *
 * Supports optional message queue output via `IQueue<Message>` and
 * standard printf-style formatting. Can be used for debugging, info,
 * warnings, errors, and fatal messages.
 *
 * Messages can be sent to a queue (e.g., `MockQueue` or System V queue)
 * or handled in any custom way in the implementation of the queue.
 */
class Logger {
public:
    /**
     * @brief Construct a logger.
     * @param level Minimum message level to log.
     * @param msq Optional queue to send messages to.
     * @param key Optional IPC key associated with the logger.
     */
    Logger(const MessageLevel level, IQueue<Message> *msq = nullptr, const key_t key = 0x0)
        : _key(key), _msq(msq), _level(level) {}

    /** @brief Log a debug-level message (lowest severity). */
    void debug(const char *fmt, ...) const;

    /** @brief Log an informational message. */
    void info(const char *fmt, ...) const;

    /** @brief Log a warning-level message. */
    void warn(const char *fmt, ...) const;

    /** @brief Log an error-level message. */
    void error(const char *fmt, ...) const;

    /** @brief Log a fatal error message and throw an exception */
    void fatal(const char *fmt, ...) const;

    /**
     * @brief Set the message queue to use for logging.
     * @param msq Queue to send messages to.
     */
    void setQueue(IQueue<Message> *msq) { _msq = msq; }

    /**
     * @brief Get the IPC key associated with this logger.
     * @return key_t value
     */
    [[nodiscard]] key_t key() const { return _key; }

    /**
     * @brief Get the minimum message level.
     * @return MessageLevel value
     */
    [[nodiscard]] MessageLevel level() const { return _level; }

private:
    /**
     * @brief Format a message string using printf-style arguments.
     * @param fmt Format string.
     * @param args Variadic argument list.
     * @return Formatted string.
     */
    static std::string _format(const char *fmt, va_list args);

    key_t _key;                  /**< IPC key associated with the logger */
    IQueue<Message> *_msq;       /**< Optional message queue for logging */
    MessageLevel _level;         /**< Minimum message level to log */
};
#endif //PROJEKT_LOGGER_H