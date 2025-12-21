//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_LOGGER_H
#define PROJEKT_LOGGER_H
#include <stdarg.h>

#include "IQueue.h"
#include "objects/Message.h"

class Logger {
public:
    Logger(const MessageLevel level, IQueue<Message> *msq = nullptr, const key_t key = 0x0): _key(key), _msq(msq), _level(level) {};
    void debug(const char *fmt, ...) const;
    void info(const char *fmt, ...) const;
    void warn(const char *fmt, ...) const;
    void error(const char *fmt, ...) const;
    void fatal(const char *fmt, ...) const;

    void setQueue(IQueue<Message> *msq) {_msq = msq;}
    [[nodiscard]] key_t key() const {return _key;}
    [[nodiscard]] MessageLevel level() const {return _level;}
private:
    static std::string _format(const char *fmt, va_list args);

    key_t _key;
    IQueue<Message> *_msq;
    MessageLevel _level;
};

#endif //PROJEKT_LOGGER_H