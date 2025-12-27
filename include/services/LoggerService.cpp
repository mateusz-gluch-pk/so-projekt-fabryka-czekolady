//
// Created by mateusz on 27.12.2025.
//

#include "LoggerService.h"

LoggerService::LoggerService(const std::string &name, const MessageLevel level):
    _key(make_key(LOGGING_DIR, name)),
    _msq(_key, true),
    _rootLogger(level, &_msq, _key),
    _collector(std::make_unique<LogCollector>(name, _rootLogger, true), _rootLogger) {
    _collector.run();
}

LoggerService::~LoggerService() {
    _collector.stop();
}

Logger & LoggerService::get() {return _rootLogger;}
