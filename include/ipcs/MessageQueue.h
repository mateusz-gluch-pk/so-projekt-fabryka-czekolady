//
// Created by mateusz on 29.11.2025.
//

#ifndef PROJEKT_MESSAGEQUEUE_H
#define PROJEKT_MESSAGEQUEUE_H
#include <sstream>
#include <sys/types.h>
#include <sys/msg.h>

#include "logger/MockQueue.h"
#include "logger/IQueue.h"
#include "logger/Logger.h"
#include "objects/Message.h"

#define MSQ_PERMS 0644
#define MSQ_LOG_LEVEL MessageLevel::DEBUG

template <typename T>
class MessageQueue : public IQueue<T> {
public:
    static MessageQueue attach(const key_t key) { return MessageQueue(key, false); };
    static MessageQueue create(const key_t key) { return MessageQueue(key, true); };

    explicit MessageQueue(key_t key, bool create);

    ~MessageQueue() override;

    // copying is prohibited!
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue & operator=(const MessageQueue &) = delete;
    // but move is allowed
    MessageQueue(MessageQueue && other) noexcept;
    MessageQueue &operator=(MessageQueue && other) noexcept;

    void send(T data) const override;
    void receive(T *data) const override;

private:
    [[nodiscard]] std::string _msg(const std::string &msg) const {
        std::ostringstream ss;
        ss << _msqid;
        return "ipcs/MessageQueue/" + ss.str() + ":\t" + msg;
    }

    IQueue<Message> *_local_msq = nullptr;
    Logger *_log;
    int _msqid = -1;
    bool _owner;
};

template<typename T>
MessageQueue<T>::MessageQueue(const key_t key, const bool create) {
    int flags = create ? (IPC_CREAT | IPC_EXCL | MSQ_PERMS) : MSQ_PERMS;

    _msqid = msgget(key, flags);
    if (_msqid == -1) {
        perror("Cannot create message queue");
        exit(errno);
    }

    if constexpr (std::is_same_v<T, Message>) {
        _log = new Logger(MSQ_LOG_LEVEL, this);
    } else {
        _local_msq = new MockQueue<Message>();
        _log = new Logger(MSQ_LOG_LEVEL, _local_msq);
        _log->info(_msg("Message is not Message - creating local queue").c_str());
    }

    _owner = create;
    if (_owner) {
        _log->info(_msg("Created with key %x").c_str(), key);
    } else {
        _log->info(_msg("Attached with key %x").c_str(), key);
    }
}


template<typename T>
MessageQueue<T>::~MessageQueue() {
    if (_msqid != -1 && _owner) {
        const int ret = msgctl(_msqid, IPC_RMID, nullptr);
        if (ret == -1) {
            _log->fatal(_msg("Cannot delete; errno: %d").c_str(), errno);
        }
        _log->info(_msg("Deleted").c_str());
    }

    if (_local_msq != nullptr) {
        _log->info(_msg("Deleted local").c_str());
        delete _local_msq;
        _local_msq = nullptr;
    }

    delete _log;
    _log = nullptr;
}

template<typename T>
MessageQueue<T>::MessageQueue(MessageQueue &&other) noexcept:
    _msqid(other._msqid),
    _owner(other._owner),
    _log(other._log) {
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
    _log = other._log;
    other._msqid = -1;
    other._owner = false;
    return *this;
}

template<typename T>
void MessageQueue<T>::send(T data) const {
    const void *data_ptr = reinterpret_cast<void *>(&data);
    const int result = msgsnd(_msqid, data_ptr, sizeof(T) - sizeof(long), IPC_NOWAIT);
    if (result == -1) {
        _log->fatal(_msg("Cannot send; errno: %d").c_str(), errno);
    }
    _log->debug(_msg("Sent").c_str());
}

template<typename T>
void MessageQueue<T>::receive(T *data) const {
    // void *data_ptr = malloc(sizeof(T));
    const int msize = msgrcv(_msqid, data, sizeof(T) - sizeof(long), 0, 0);

    if (msize == -1) {
        // free(data_ptr);
        // data_ptr = nullptr;
        _log->fatal(_msg("Cannot receive; errno: %d").c_str(), errno);
        return;
    }

    // *data = *(static_cast<T *>(data_ptr));
    _log->debug(_msg("Received").c_str());
    // free(data_ptr);
    // data_ptr = nullptr;
}

#endif //PROJEKT_MESSAGEQUEUE_H
