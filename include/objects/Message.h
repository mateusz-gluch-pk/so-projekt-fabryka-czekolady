//
// Created by mateusz on 1.12.2025.
//

#ifndef PROJEKT_MESSAGE_H
#define PROJEKT_MESSAGE_H
#include <string>

#define MAX_MESSAGE_LENGTH 255

enum MessageLevel {
    INVALID = -1,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Message {
    public:
        long mtype;

        Message(): _message(""), _level(MessageLevel::INVALID), _timestamp(0) {};
        Message(MessageLevel level, const std::string &message);

        // to logfmt
        std::string string() const;

        // to JSON
        // std::string serialize();

    private:
        std::string _level_str() const;

        char _message[MAX_MESSAGE_LENGTH];
        MessageLevel _level;
        time_t _timestamp;
};

std::ostream &operator<<(std::ostream &os, const Message &msg);

#endif //PROJEKT_MESSAGE_H