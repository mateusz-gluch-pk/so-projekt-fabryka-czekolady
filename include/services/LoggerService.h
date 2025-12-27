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
        LoggerService(const std::string &name, const MessageLevel level):
        _key(make_key(LOGGING_DIR, name)),
        _msq(_key, true),
        _rootLogger(level, &_msq, _key),
        _collector(std::make_unique<LogCollector>(name, _rootLogger, true), _rootLogger) {
            _collector.run();
        }

        ~LoggerService() {
            _collector.stop();
        }

        Logger &get() {return _rootLogger;}

    private:
        key_t _key;
        MessageQueue<Message> _msq;
        Logger _rootLogger;
        ProcessController _collector;
};

#endif //FACTORY_LOGGERSERVICE_H