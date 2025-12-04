//
// Created by mateusz on 26.11.2025.
//

#include "LogCollector.h"

#include <iostream>

namespace stime = std::chrono;
namespace sthr = std::this_thread;

LogCollector::LogCollector(MessageQueue<Message> *msq, const std::string &filename, const bool tty):
    _msq(msq), _filename(filename), _tty(tty), _running(true), _paused(false), _reload(false) {
    _file = _open_file();
}

LogCollector::~LogCollector() {_close_file();}

void LogCollector::run() {
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
