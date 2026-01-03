//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_LOGGERSERVICE_H
#define FACTORY_LOGGERSERVICE_H
#include "actors/LogCollector.h"
#include "ipcs/key.h"
#include "ipcs/MessageQueue.h"
#include "processes/ProcessController.h"

// RAII - style service for logging
// Spawns 1 process - log collector
// Spawns 1 ipc - MSQ for logs
// Use as source for root logger
class LoggerService {
    public:
        LoggerService(const std::string &name, MessageLevel level);
        ~LoggerService();
        Logger &get();
        [[nodiscard]] const SharedVector<Message, LOGGING_BUFFER_SIZE> *logs() const {return _buffer.get();};

    private:
        key_t _key;
        MessageQueue<Message> _msq;
        Logger _rootLogger;

        SharedMemory<SharedVector<Message, LOGGING_BUFFER_SIZE>> _buffer;

        ProcessController _collector;
};

#endif //FACTORY_LOGGERSERVICE_H