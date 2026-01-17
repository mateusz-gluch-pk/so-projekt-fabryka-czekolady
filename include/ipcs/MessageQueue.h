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

#define MSQ_PERMS 0600
#define MSQ_LOG_LEVEL MessageLevel::INFO

/**
 * @brief RAII wrapper for System V message queues.
 * This class uses **RAII** (Resource Acquisition Is Initialization):
 * the semaphore is automatically removed from the system when the owning
 * instance is destroyed.
 *
 * This class manages a System V message queue, automatically removing it
 * if this instance owns it. Ownership is tracked by the `_owner` flag:
 * - If `create=true`, the instance **owns** the queue and will delete it in the destructor.
 * - If `create=false` (attach), the instance **does not own** the queue and will leave it in the system.
 *
 * Copying is prohibited to avoid multiple owners; move semantics are supported.
 *
 * @tparam T Type of message stored in the queue (must contain `long mtype` as required by System V message queues).
 */
template <typename T>
class MessageQueue : public IQueue<T> {
public:
    /**
     * @brief Attach to an existing message queue.
     * @param key System V IPC key.
     * @return MessageQueue object representing the attached queue.
     */
    static MessageQueue attach(const key_t key) { return MessageQueue(key, false); }

    /**
     * @brief Create a new message queue.
     * @param key System V IPC key.
     * @return MessageQueue object representing the created queue.
     */
    static MessageQueue create(const key_t key) { return MessageQueue(key, true); }

    /**
     * @brief Construct a message queue.
     * @param key System V IPC key.
     * @param create If true, create the queue; if false, attach to existing.
     * @throw std::runtime_error via Logger::fatal if creation fails.
     */
    explicit MessageQueue(key_t key, bool create);

    /**
     * @brief Destructor removes the queue if owned and cleans up resources.
     */
    ~MessageQueue() override;

    // Copying prohibited
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue & operator=(const MessageQueue &) = delete;

    // Move allowed
    MessageQueue(MessageQueue && other) noexcept;
    MessageQueue &operator=(MessageQueue && other) noexcept;

    /**
     * @brief Send a message to the queue.
     * @param data Message to send.
     * @note Uses `msgsnd`; logs errors if send fails.
     */
    void send(T data) const override;

    /**
     * @brief Receive a message from the queue.
     * @param data Pointer to store received message.
     * @note Uses `msgrcv`; logs errors if receive fails.
     */
    void receive(T *data) const override;

private:
    /**
     * @brief Format a log message with queue ID.
     * @param msg Message text.
     * @return Formatted string.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const {
        std::ostringstream ss;
        ss << _msqid;
        return "ipcs/MessageQueue/" + ss.str() + ":\t" + msg;
    }

    IQueue<Message> *_local_msq = nullptr; /**< Local queue for testing/logging */
    Logger *_log;                           /**< Logger for debug/info/error messages */
    int _msqid = -1;                        /**< System V message queue ID */
    bool _owner;                             /**< True if this instance created the queue */
};

template<typename T>
MessageQueue<T>::MessageQueue(const key_t key, const bool create) {
    int flags = create ? (IPC_CREAT | IPC_EXCL | MSQ_PERMS) : MSQ_PERMS;

    _msqid = msgget(key, flags);
    if (_msqid == -1) {
        perror("Cannot create message queue");
        exit(errno);
    }

    _local_msq = new MockQueue<Message>();
    _log = new Logger(MSQ_LOG_LEVEL, _local_msq);

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
    const int result = msgsnd(_msqid, data_ptr, sizeof(T) - sizeof(long), 0);
    if (result == -1) {
        _log->error(_msg("Cannot send; errno: %d").c_str(), errno);
    }
    _log->debug(_msg("Sent").c_str());
}

template<typename T>
void MessageQueue<T>::receive(T *data) const {
    const int msize = msgrcv(_msqid, data, sizeof(T) - sizeof(long), 0, 0);

    if (msize == -1) {
        _log->error(_msg("Cannot receive; errno: %d").c_str(), errno);
        return;
    }

    _log->debug(_msg("Received").c_str());
}

#endif //PROJEKT_MESSAGEQUEUE_H
