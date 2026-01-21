//
// Created by mateusz on 26.11.2025.
//

#include "LogCollector.h"

#include <chrono>
#include <iostream>

#include "ipcs/key.h"

namespace stime = std::chrono;
namespace sthr = std::this_thread;

LogCollector::LogCollector(std::string name, Logger &log, bool tty):
    _name(std::move(name)),
    _log(log),
    _tty(tty),
    _running(true), _reloading(false), _paused(false) {
    _msq.emplace(make_key(LOGGING_DIR, _name, &log), false);
    size_t bufsize = sizeof(SharedVector<Message, LOGGING_BUFFER_SIZE>);
    _buffer.emplace(make_key(LOGGING_DIR, _name, &log), bufsize, &log, false);
    if (tty) _log.info(_msg("TTY output enabled").c_str());
    _log.info(_msg("Created").c_str());
}

LogCollector::~LogCollector() {
    _close_file();
}

void LogCollector::run(ProcessStats *stats) {
    stats->pid = getpid();
    _file = _open_file();

    while (_running || _msq->messages() > 0) {
        if (_paused) {
            stats->state = PAUSED;
            sthr::sleep_for(stime::milliseconds(10));
            continue;
        }

        if (_reloading) {
            stats->state = RELOADING;
            _reload();
            _reloading = false;
            stats->reloads++;
            continue;
        }

        stats->state = RUNNING;
        _main();
        _log.debug(_msg("Loop completed").c_str());
        stats->loops++;
    }
    stats->state = STOPPED;
}

void LogCollector::stop() {
    _log.info(_msg("Received SIGTERM - terminating").c_str());
    _running = false;
}

void LogCollector::pause() {
    _log.info(_msg("Received SIGUSR1 - pausing").c_str());
    _paused = true;
}

void LogCollector::resume() {
    _log.info(_msg("Received SIGCONT - resuming").c_str());
    _paused = false;
}

void LogCollector::reload() {
    _log.info(_msg("Received SIGHUP - reloading").c_str());
    _reloading = true;
}

void LogCollector::_main() {
    Message msg;

    _msq->receive(&msg);

    auto buf = _buffer->get();
    if (buf->size == LOGGING_BUFFER_SIZE) {
        _log.debug(_msg("Erasing beginning of buffer, %d/%d").c_str(), buf->size, buf->capacity);
        buf->erase(buf->begin());
    }
    _log.debug(_msg("Appending log to buffer").c_str());
    buf->push_back(msg);

    const auto log = msg.string();
    if (_tty) std::cout << log << std::endl;
    _file << log << std::endl;
}

void LogCollector::_reattach(Logger &log) {
    _file = _open_file();
    _log = log;
    _msq.emplace(make_key(LOGGING_DIR, _name, &log), false);

    size_t bufsize = sizeof(SharedVector<Message, LOGGING_BUFFER_SIZE>);
    _buffer.emplace(make_key(LOGGING_DIR, _name, &log), bufsize, &log, false);
}

std::string LogCollector::_msg(const std::string &msg) const {
    return "actors/LogCollector/" + _name + ":\t" + msg;
}

std::ofstream LogCollector::_open_file() const {
    stime::sys_seconds tp{stime::seconds{time(nullptr)}};
    stime::zoned_time local{stime::current_zone(), tp};
    const std::string time_str = std::format("{:%Y_%m_%d_%H_%M_%S}", local);
    const std::string filename = std::string(LOGGING_DIR) + "/" + _name + "_" +  time_str + ".log";

    auto file = std::ofstream(filename, std::ios::app);
    if (!file.is_open()) {
        _log.fatal(_msg("Could not open file").c_str());
    }
    _log.info(_msg("File %s opened").c_str(), filename.c_str());
    return file;
}

void LogCollector::_close_file() {
    if (_file.is_open()) {
        _file.close();
        std::cout << _msg("Log file closed") << std::endl;
    }
}

std::vector<std::string> LogCollector::argv() {
    auto args = std::vector<std::string>();

    args.push_back("/proc/self/exe");
    args.push_back("--worker");
    args.push_back("LogCollector");

    args.push_back("--name");
    args.push_back(_name);

    args.push_back("--tty");
    args.push_back(_tty? "1": "0");

    return args;
}