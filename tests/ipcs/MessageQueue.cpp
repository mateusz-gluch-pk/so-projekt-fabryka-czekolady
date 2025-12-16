//
// Created by mateusz on 13.12.2025.
//
#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <wait.h>
#include <ipcs/MessageQueue.h>
#include <objects/Message.h>

#include "../helpers/MockQueue.h"

constexpr key_t TEST_KEY = 0x1000;

TEST(MessageQueue, SendReceiveSameProcess) {
    MockQueue<Message> queue;
    auto mq = MessageQueue<int>::create(TEST_KEY, &queue);

    int message = 42;
    mq.send(message);

    int received = 0;
    mq.receive(&received);

    EXPECT_EQ(message, received);
}

TEST(MessageQueue, SendReceiveDifferentProcess) {
    MockQueue<Message> queue;
    auto mq = MessageQueue<int>::create(TEST_KEY, &queue);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        mq = MessageQueue<int>::attach(TEST_KEY, &queue);
        int val = 42;
        mq.send(val);
        exit(0);
    }

    int received = 0;
    bool got_message = false;
    for (int i = 0; i < 10 && !got_message; ++i) {
        try {
            mq.receive(&received);
            got_message = true;
        } catch (...) {
            usleep(100000); // 100ms
        }
    }

    EXPECT_TRUE(got_message);
    EXPECT_EQ(received, 42);

    int status;
    waitpid(pid, &status, 0);
}

TEST(MessageQueue, MultipleMessage) {
    MockQueue<Message> queue;
    auto mq = MessageQueue<int>::create(TEST_KEY, &queue);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        mq = MessageQueue<int>::attach(TEST_KEY, &queue);
        for (int i = 1; i <= 5; ++i) {
            mq.send(i*10);
        }
        _exit(0);
    }

    int status;
    waitpid(pid, &status, 0);

    for (int i = 1; i <= 5; ++i) {
        int received = 0;
        mq.receive(&received);
        EXPECT_EQ(received, i*10);
    }
}
