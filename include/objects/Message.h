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
        long mtype = 1;

        Message();
        Message(MessageLevel level, const std::string &message);

        // to logfmt
        std::string string() const;

        // to JSON
        // std::string serialize();

    // private:
        std::string _level_str() const;

        char _message[MAX_MESSAGE_LENGTH];
        MessageLevel _level;
        pid_t _pid;
        time_t _timestamp;
};

std::ostream &operator<<(std::ostream &os, const Message &msg);

#endif //PROJEKT_MESSAGE_H