//
// Created by mateusz on 1.12.2025.
//

#ifndef PROJEKT_MESSAGE_H
#define PROJEKT_MESSAGE_H
#include <string>

#define MAX_MESSAGE_LENGTH 255

/**
 * @brief Levels of messages for logging or error reporting.
 */
enum MessageLevel {
    INVALID = -1, ///< Invalid or uninitialized level
    DEBUG,        ///< Debug-level message
    INFO,         ///< Informational message
    WARNING,      ///< Warning message
    ERROR,        ///< Error message
    FATAL         ///< Fatal error
};

/**
 * @brief Represents a log message with metadata: PID, timestamp, Log Level.
 */
class Message {
public:
    long mtype = 1; ///< Message type, typically used in message queues

    /// Default constructor (empty message)
    Message();

    /**
     * @brief Construct a message with a level and text.
     * @param level   Severity level of the message
     * @param message Message text
     */
    Message(MessageLevel level, const std::string &message);

    /**
     * @brief Convert message to a logfmt string.
     * @return Message formatted as a log string
     */
    std::string string() const;

    /// Convert message to JSON string (not implemented)
    // std::string serialize();

private:
    /**
     * @brief Get the string representation of the message level.
     * @return Level as a string (e.g., "DEBUG", "ERROR")
     */
    std::string _level_str() const;

    char _message[MAX_MESSAGE_LENGTH]; ///< Null-terminated message text
    MessageLevel _level;               ///< Severity level
    pid_t _pid;                         ///< Process ID of the sender
    time_t _timestamp;                  ///< Timestamp of the message
};

std::ostream &operator<<(std::ostream &os, const Message &msg);

#endif //PROJEKT_MESSAGE_H