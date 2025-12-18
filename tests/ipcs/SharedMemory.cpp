//
// Created by mateusz on 14.12.2025.
//

#include <sys/types.h>
#include <gtest/gtest.h>

#include <ipcs/SharedMemory.h>
#include <logger/Logger.h>

#include "logger/MockQueue.h"

constexpr key_t TEST_KEY = 0x1002;

struct TestData {
    int a;
    int b;
};

TEST(SharedMemory, CreateInitializesMemory)
{
    auto log = Logger(MessageLevel::DEBUG, static_cast<IQueue<Message>>(MockQueue<Message>()));
    auto shm = SharedMemory<TestData>::create(TEST_KEY, sizeof(TestData), &log);

    EXPECT_EQ(shm->a, 0);
    EXPECT_EQ(shm->b, 0);
}

TEST(SharedMemory, WriteAndRead)
{
    auto log = Logger(MessageLevel::DEBUG, static_cast<IQueue<Message>>(MockQueue<Message>()));
    auto shm = SharedMemory<TestData>::create(TEST_KEY, sizeof(TestData), &log);

    shm->a = 42;
    shm->b = 13;

    EXPECT_EQ((*shm).a, 42);
    EXPECT_EQ((*shm).b, 13);
}

TEST(SharedMemory, AttachReadsExistingData)
{
    auto log = Logger(MessageLevel::DEBUG, static_cast<IQueue<Message>>(MockQueue<Message>()));

    {
        auto creator = SharedMemory<TestData>::create(TEST_KEY, sizeof(TestData), &log);
        creator->a = 7;
        creator->b = 9;

        auto attached= SharedMemory<TestData>::attach(TEST_KEY, sizeof(TestData), &log);
        EXPECT_EQ(attached->a, 7);
        EXPECT_EQ(attached->b, 9);
    }
}

// TEST(SharedMemory, ParentCreatesChildAttaches)
// {
//     auto log = Logger(MessageLevel::DEBUG, static_cast<IQueue<Message>>(MockQueue<Message>()));
//     auto parent = SharedMemory<TestData>::create(TEST_KEY, sizeof(TestData), &log);
//     parent->a = 42;
//     parent->b = 99;
//
//     pid_t pid = fork();
//     ASSERT_NE(pid, -1);
//
//     if (pid == 0) {
//         try {
//             auto child =
//                 SharedMemory<TestData>::attach(key, sizeof(TestData), &log);
//
//             if (child->a == 42 && child->b == 99)
//                 _exit(0);
//             else
//                 _exit(1);
//         } catch (...) {
//             _exit(2);
//         }
//     }
//
//     // PARENT
//     int status = 0;
//     waitpid(pid, &status, 0);
//
//     ASSERT_TRUE(WIFEXITED(status));
//     EXPECT_EQ(WEXITSTATUS(status), 0);
// }