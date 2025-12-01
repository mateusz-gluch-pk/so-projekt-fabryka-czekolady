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
    MessageQueue(key_t key, bool create=true);
    ~MessageQueue();

    // copying is prohibited!
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue & operator=(const MessageQueue &) = delete;
    // but move is allowed
    MessageQueue(MessageQueue && other) noexcept;
    MessageQueue &operator=(MessageQueue && other) noexcept;

    void send(T data);
    void receive(T *data);

private:
    int _msqid = -1;
    bool _owner;
};

template<typename T>
MessageQueue<T>::MessageQueue(key_t key, bool create) {
    int flags = create ? (IPC_CREAT | MSQ_PERMS) : MSQ_PERMS;

    _msqid = msgget(key, flags);
    if (_msqid == -1)
        throw std::runtime_error("msgget failed");

    _owner = create;
}


template<typename T>
MessageQueue<T>::~MessageQueue() {
    if (_owner) {
        msgctl(_msqid, IPC_RMID, nullptr);
    }
}

template<typename T>
MessageQueue<T>::MessageQueue(MessageQueue &&other) noexcept:
    _msqid(other._msqid),
    _owner(other._owner) {
    other._msqid = -1;
    other._owner = false;
}

template<typename T>
MessageQueue<T> & MessageQueue<T>::operator=(MessageQueue<T> &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    _msqid = other._msqid;
    _owner = other._owner;
    other._msqid = -1;
    other._owner = false;
    return *this;
}

template<typename T>
void MessageQueue<T>::send(T data) {
    const void *data_ptr = reinterpret_cast<void *>(&data);
    const int result = msgsnd(_msqid, data_ptr, sizeof(T), IPC_NOWAIT);
    if (result == -1) {
        throw std::runtime_error("msgsnd failed");
    }
}

template<typename T>
void MessageQueue<T>::receive(T *data) {
    void *data_ptr = reinterpret_cast<void *>(data);
    msgrcv(_msqid, data_ptr, sizeof(T), IPC_NOWAIT, 0);
}


#endif //PROJEKT_MESSAGEQUEUE_H
