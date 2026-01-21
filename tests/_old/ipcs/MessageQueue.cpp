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

class IntMessage {
public:
    long mtype = 1;
    int payload = 0;
};

TEST(MessageQueue, SendReceiveSameProcess) {
    auto mq = MessageQueue<IntMessage>::create(TEST_KEY);

    IntMessage message(1, 42);
    mq.send(message);

    IntMessage received(0, 0);
    mq.receive(&received);

    EXPECT_EQ(message.payload, received.payload);
}

TEST(MessageQueue, SendReceiveDifferentProcess) {
    auto mq = MessageQueue<IntMessage>::create(TEST_KEY);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        auto mq = MessageQueue<IntMessage>::attach(TEST_KEY);
        IntMessage message(1, 42);
        mq.send(message);
        exit(0);
    }

    IntMessage received(0, 0);
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
    EXPECT_EQ(received.payload, 42);

    int status;
    waitpid(pid, &status, 0);
}

TEST(MessageQueue, MultipleMessage) {
    auto mq = MessageQueue<IntMessage>::create(TEST_KEY);

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        auto mq = MessageQueue<IntMessage>::attach(TEST_KEY);
        for (int i = 1; i <= 5; ++i) {
            IntMessage message(1, i*10);
            mq.send(message);
        }
        _exit(0);
    }

    int status;
    waitpid(pid, &status, 0);

    for (int i = 1; i <= 5; ++i) {
        IntMessage received(0, 0);
        mq.receive(&received);
        EXPECT_EQ(received.payload, i*10);
    }
}
