//
// Created by mateusz on 13.12.2025.
//
#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <wait.h>
#include <ipcs/MessageQueue.h>

constexpr key_t TEST_KEY = 0x1000;

TEST(MessageQueue, SendReceiveSameProcess) {
    auto mq = MessageQueue<int>::create(TEST_KEY);

    int message = 42;
    mq.send(message);

    int received = 0;
    mq.receive(&received);

    EXPECT_EQ(message, received);
}

TEST(MessageQueue, SendReceiveDifferentProcess) {
    auto mq = MessageQueue<int>::create(TEST_KEY);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        mq = MessageQueue<int>::attach(TEST_KEY);
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
    auto mq = MessageQueue<int>::create(TEST_KEY);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        mq = MessageQueue<int>::attach(TEST_KEY);
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
