//
// Created by mateusz on 1.12.2025.
//

#ifndef PROJEKT_MESSAGE_H
#define PROJEKT_MESSAGE_H
#include <string>

#define MAX_MESSAGE_LENGTH 255

enum MessageLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Message {
    public:
        Message(MessageLevel level, const std::string &message);

        // to logfmt
        std::string string();

        // to JSON
        // std::string serialize();

    private:
        std::string _level_str() const;

        char _message[MAX_MESSAGE_LENGTH];
        MessageLevel _level;
        time_t _timestamp;
};

#endif //PROJEKT_MESSAGE_H