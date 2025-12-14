//
// Created by mateusz on 29.11.2025.
//

#ifndef PROJEKT_MESSAGEQUEUE_H
#define PROJEKT_MESSAGEQUEUE_H
#include <stdexcept>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "logger/IQueue.h"
#include "logger/Logger.h"
#include "objects/Message.h"

#define MSQ_PERMS 0644
#define MSQ_LOG_LEVEL MessageLevel::INFO

template <typename T>
class MessageQueue : public IQueue<T> {
public:
    explicit MessageQueue(key_t key, bool create, IQueue<Message> *external_msq);

    ~MessageQueue() override;

    // copying is prohibited!
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue & operator=(const MessageQueue &) = delete;
    // but move is allowed
    MessageQueue(MessageQueue && other) noexcept;
    MessageQueue &operator=(MessageQueue && other) noexcept;

    void send(T data) override;
    void receive(T *data) override;

private:
    Logger *_log;
    int _msqid = -1;
    bool _owner;
};

template<typename T>
MessageQueue<T>::MessageQueue(const key_t key, const bool create, IQueue<Message> *external_msq) {
    int flags = create ? (IPC_CREAT | MSQ_PERMS) : MSQ_PERMS;

    _msqid = msgget(key, flags);
    if (_msqid == -1) {
        perror("Cannot create message queue");
        exit(errno);
    }

    if (external_msq != nullptr) {
        _log = new Logger(MSQ_LOG_LEVEL, external_msq);
    } else if constexpr (std::is_same<T, Message>::value) {
        _log = new Logger(MSQ_LOG_LEVEL, this);
    } else {
        perror("Cannot create message queue");
        exit(errno);
    }

    _owner = create;
    if (_owner) {
        _log->info("Created message queue %d", _msqid);
    } else {
        _log->info("Attached to message queue %d", _msqid);
    }
}


template<typename T>
MessageQueue<T>::~MessageQueue() {
    if (_owner) {
        const int ret = msgctl(_msqid, IPC_RMID, nullptr);
        if (ret == -1) {
            _log->fatal("Cannot remove message queue %d", _msqid);
        }
        _log->info("Deleted message queue %d", _msqid);
    }

    delete _log;
    _log = nullptr;
}

template<typename T>
MessageQueue<T>::MessageQueue(MessageQueue &&other) noexcept:
    _msqid(other._msqid),
    _owner(other._owner),
    _log(other._logger) {
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
    _log = other._logger;
    other._msqid = -1;
    other._owner = false;
    return *this;
}

template<typename T>
void MessageQueue<T>::send(T data) {
    const void *data_ptr = reinterpret_cast<void *>(&data);
    const int result = msgsnd(_msqid, data_ptr, sizeof(T), IPC_NOWAIT);
    if (result == -1) {
        _log->fatal("Cannot send message to message queue %d; errno: %d", _msqid, errno);
    }
    _log->info("Sent message to message queue %d", _msqid);
}

template<typename T>
void MessageQueue<T>::receive(T *data) {
    void *data_ptr = malloc(sizeof(T));
    const int msize = msgrcv(_msqid, data_ptr, sizeof(T), 0, 0);
    if (msize == -1) {
        free(data_ptr);
        _log->fatal("Cannot receive message from message queue %d; errno: %d", _msqid, errno);
    }

    *data = *(static_cast<T *>(data_ptr));
    _log->info("Received message from message queue %d", _msqid);
    free(data_ptr);
}

#endif //PROJEKT_MESSAGEQUEUE_H
