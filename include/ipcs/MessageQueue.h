//
// Created by mateusz on 29.11.2025.
//

#ifndef PROJEKT_MESSAGEQUEUE_H
#define PROJEKT_MESSAGEQUEUE_H
#include <stdexcept>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#define MSQ_PERMS 0644

template <typename T>
class MessageQueue {
public:
    MessageQueue(key_t key, bool create=true) {
        int flags = create ? (IPC_CREAT | MSQ_PERMS) : MSQ_PERMS;

        _msqid = msgget(key, flags);
        if (_msqid == -1)
            throw std::runtime_error("msgget failed");

        _owner = create;
    };

    ~MessageQueue() {

    };

    void send(T data);
    T receive();

private:
    int _msqid = -1;
    bool _owner;
};


#endif //PROJEKT_MESSAGEQUEUE_H