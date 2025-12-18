//
// Created by mateusz on 14.12.2025.
//

#include "ipcs/Semaphore.h"
#include <gtest/gtest.h>

#include "../helpers/MockQueue.h"

#define UTICK 1e6

constexpr key_t TEST_KEY = 0x1001;

TEST(Semaphore, LockUnlockSingleProcess) {
    const auto mq = MockQueue<Message>();
    auto log = Logger(MessageLevel::DEBUG, static_cast<IQueue<Message>>(mq));
    auto sem = Semaphore::create(TEST_KEY, &log);

    sem.lock();
    ASSERT_EQ(0, sem.value());
    sem.unlock();
    ASSERT_EQ(1, sem.value());
}

TEST(Semaphore, LockWaitMultiProcess) {
    auto mq = MockQueue<Message>();
    auto log = Logger(MessageLevel::DEBUG, static_cast<IQueue<Message>>(mq));
    auto sem = Semaphore::create(TEST_KEY, &log);

    pid_t pid = fork();
    if (pid == 0) {
        sem = Semaphore::attach(TEST_KEY, &log);
        auto t0 = time(nullptr);
        sem.lock();               // must block
        auto t1 = time(nullptr);

        ASSERT_GE(t1 - t0, 1);
        sem.unlock();
        exit(0);
    }

    sem.lock();
    sleep(1);
    sem.unlock();
    waitpid(pid, nullptr, 0);
}

