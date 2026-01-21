//
// Created by mateusz on 27.12.2025.
//

#include "LoggerService.h"

constexpr size_t bufsize = sizeof(SharedVector<Message, LOGGING_BUFFER_SIZE>) + sizeof(Message) * LOGGING_BUFFER_SIZE;

LoggerService::LoggerService(const std::string &name, const MessageLevel level, bool tty):
    _key(make_key(LOGGING_DIR, name)),
    _msq(_key, true),
    _rootLogger(level, &_msq, _key),
    _buffer(_key, bufsize, &_rootLogger, true),
    _collector(std::make_unique<LogCollector>(name, _rootLogger, tty), _rootLogger) {
    _collector.run();
}

LoggerService::~LoggerService() {
    _collector.stop();
}

Logger & LoggerService::get() {return _rootLogger;}
