//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_LOGGERSERVICE_H
#define FACTORY_LOGGERSERVICE_H
#include "actors/LogCollector.h"
#include "ipcs/key.h"
#include "ipcs/MessageQueue.h"
#include "processes/ProcessController.h"

/**
 * @brief RAII-style service for logging that manages a log collector and IPC.
 *
 * Spawns a log collector process and a message queue for logging.
 * Can be used as the source for the root logger.
 */
class LoggerService {
public:
    /**
     * @brief Constructs a LoggerService.
     * @param name Name of the logger service.
     * @param level Logging level for messages.
     * @throws std::runtime_error if initialization fails.
     */
    LoggerService(const std::string &name, MessageLevel level, bool tty = false);

    /**
     * @brief Destructor; stops the collector process and cleans up resources.
     */
    ~LoggerService();

    /**
     * @brief Returns a reference to the root logger.
     * @return Reference to Logger object.
     */
    Logger &get();

    /**
     * @brief Returns a pointer to the shared memory log buffer.
     * @return Pointer to SharedVector of messages (read-only).
     */
    [[nodiscard]] const SharedVector<Message, LOGGING_BUFFER_SIZE> *logs() const { return _buffer.get(); }

private:
    key_t _key;                                              ///< IPC key for message queue
    MessageQueue<Message> _msq;                              ///< Message queue for log messages
    Logger _rootLogger;                                      ///< Root logger instance
    SharedMemory<SharedVector<Message, LOGGING_BUFFER_SIZE>> _buffer; ///< Shared memory buffer for logs
    ProcessController _collector;                             ///< Process controller for log collector
};

#endif //FACTORY_LOGGERSERVICE_H