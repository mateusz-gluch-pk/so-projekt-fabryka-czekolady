//
// Created by mateusz on 26.11.2025.
//

#include "LogCollector.h"

#include <iostream>

#include "ipcs/key.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;

LogCollector::LogCollector(std::string name, MessageLevel level, bool tty):
    _name(name),
    _tty(tty),
    _running(true), _reloading(false), _paused(false)
{
    MockQueue<Message> local_mq;
    Logger local_logger(level, &local_mq);
    key_t key = make_key(LOGGING_DIR, name, local_logger);

    // this queue persists as long as main process
    _msq.emplace(key, true);
    _log = Logger(level, _msq, key);
}

LogCollector::~LogCollector() {
    _close_file();
}

void LogCollector::run(ProcessStats &stats, Logger &log) {
    while (_running) {
        if (_paused) {
            // safe sleep for 0.1s
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (_reloading) {
            _reload();
            _reloading = false;
        }

        _main();
    }
}

void LogCollector::stop() { _running = false; }
void LogCollector::pause() { _paused = true; }
void LogCollector::resume() { _paused = false; }
void LogCollector::reload() { _reloading = true; }

void LogCollector::_main() {
    Message msg;
    _msq->receive(&msg);
    _write_log(msg);
}

std::ofstream LogCollector::_open_file() const {
    auto file = std::ofstream(_filename, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("LogCollector: could not open log file");
    }
    return file;
}

void LogCollector::_close_file() {
    if (_file.is_open()) {
        _file.close();
    }
}

void LogCollector::_write_log(Message &msg) {
    const auto log = msg.string();
    if (_tty) {
        std::cout << log;
    }
    _file << log;
}
