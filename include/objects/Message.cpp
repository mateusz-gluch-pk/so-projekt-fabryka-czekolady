//
// Created by mateusz on 1.12.2025.
//

#include "Message.h"

#include <cstring>
#include <ctime>
#include <stdexcept>

Message::Message(const MessageLevel level, const std::string &message) {
    if (message.length() >= MAX_MESSAGE_LENGTH) {
        throw new std::length_error("Message too long");
    };

    _timestamp = time(nullptr);
    _level = level;
    strcpy(_message, message.c_str());
}

std::string Message::string() const {
    const std::string time_str = std::to_string(_timestamp);
    const std::string level_str = _level_str();
    return time_str + " " + level_str + " " + std::string(_message);
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
    }
    return "UNK";
}

std::ostream &operator<<(std::ostream &os, const Message &msg) {
    os << msg.string();
    return os;
}