//
// Created by mateusz on 3.12.2025.
//

#include "Logger.h"

#include <stdexcept>

void Logger::debug(const char *fmt, ...) const {
    if (_level > MessageLevel::DEBUG) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    const std::string payload = _format(fmt, args);
    va_end(args);
    const auto msg = Message(MessageLevel::DEBUG, payload);
    _msq.send(msg);
}

void Logger::info(const char *fmt, ...) const {
    if (_level > MessageLevel::INFO) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    const std::string payload = _format(fmt, args);
    va_end(args);
    const auto msg = Message(MessageLevel::INFO, payload);
    _msq.send(msg);
}

void Logger::warn(const char *fmt, ...) const {
    if (_level > MessageLevel::WARNING) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    const std::string payload = _format(fmt, args);
    va_end(args);
    const auto msg = Message(MessageLevel::WARNING, payload);
    _msq.send(msg);
}

void Logger::error(const char *fmt, ...) const {
    if (_level > MessageLevel::ERROR) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    const std::string payload = _format(fmt, args);
    va_end(args);
    const auto msg = Message(MessageLevel::ERROR, payload);
    _msq.send(msg);
    perror(msg.string().c_str());
}

void Logger::fatal(const char *fmt, ...) const {
    if (_level > MessageLevel::FATAL) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    const std::string payload = _format(fmt, args);
    va_end(args);
    const auto msg = Message(MessageLevel::FATAL, payload);
    _msq.send(msg);
    perror(msg.string().c_str());
    throw std::runtime_error(payload);
}

std::string Logger::_format(const char *fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    const int size = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    std::string result(size, '\0');
    std::vsnprintf(result.data(), size + 1, fmt, args);
    return result;
}
