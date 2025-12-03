//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_LOGGER_H
#define PROJEKT_LOGGER_H
#include <stdarg.h>

#include "Message.h"
#include "ipcs/MessageQueue.h"


class Logger {
public:
    Logger(const MessageLevel level, MessageQueue<Message> *msq): _msq(msq), _level(level) {};
    void debug(const char *fmt, ...) const {
        if (_level > MessageLevel::DEBUG) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        const std::string payload = _format(fmt, args);
        va_end(args);
        _send(MessageLevel::DEBUG, payload);
    };

    void info(const char *fmt, ...) const {
        if (_level > MessageLevel::INFO) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        const std::string payload = _format(fmt, args);
        va_end(args);
        _send(MessageLevel::INFO, payload);
    };

    void warn(const char *fmt, ...) const {
        if (_level > MessageLevel::WARNING) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        const std::string payload = _format(fmt, args);
        va_end(args);
        _send(MessageLevel::WARNING, payload);
    };

    void error(const char *fmt, ...) const {
        if (_level > MessageLevel::ERROR) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        const std::string payload = _format(fmt, args);
        va_end(args);
        _send(MessageLevel::ERROR, payload);
    };

    void fatal(const char *fmt, ...) const {
        if (_level > MessageLevel::FATAL) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        const std::string payload = _format(fmt, args);
        va_end(args);
        _send(MessageLevel::FATAL, payload);
    };

private:
    static std::string _format(const char *fmt, va_list args) {
        va_list args_copy;
        va_copy(args_copy, args);
        const int size = std::vsnprintf(nullptr, 0, fmt, args_copy);
        va_end(args_copy);

        std::string result(size, '\0');
        std::vsnprintf(result.data(), size + 1, fmt, args);
        return result;
    };

    void _send(MessageLevel level, const std::string &payload) const {
        const auto msg = Message(level, payload);
        _msq->send(msg);
    };

    MessageQueue<Message> *_msq;
    MessageLevel _level;
};


#endif //PROJEKT_LOGGER_H