//
// Created by mateusz on 1.12.2025.
//

#include "Message.h"

#include <cstring>
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <unistd.h>

namespace ts = std::chrono;

Message::Message(): mtype(0), _message(""), _level(MessageLevel::INVALID), _pid(0), _timestamp(0) {}

Message::Message(const MessageLevel level, const std::string &message) {
    if (message.length() >= MAX_MESSAGE_LENGTH) {
        throw new std::length_error("Message too long");
    };

    _timestamp = time(nullptr);
    _level = level;
    _pid = getpid();
    strcpy(_message, message.c_str());
}

std::string Message::string() const {
    ts::sys_seconds tp{ts::seconds{_timestamp}};
    ts::zoned_time local{ts::current_zone(), tp};

    const std::string time_str = std::format("{:%Y-%m-%d %H:%M:%S}", local);
    const std::string level_str = _level_str();
    return time_str + "\t" + level_str + "\t[" + std::to_string(_pid) + "]\t" + std::string(_message);
}

std::string Message::_level_str() const {
    switch (_level) {
        case MessageLevel::DEBUG:
            return "DBG";
        case MessageLevel::INFO:
            return "INF";
        case MessageLevel::WARNING:
            return "WAR";
        case MessageLevel::ERROR:
            return"ERR";
        case MessageLevel::FATAL:
            return "FTL";
        case INVALID:
            return "INV";
    }
    return "UNK";
}

std::ostream &operator<<(std::ostream &os, const Message &msg) {
    os << msg.string();
    return os;
}