//
// Created by mateusz on 14.12.2025.
//

#include "ipcs/Semaphore.h"
#include <gtest/gtest.h>

#include "../helpers/MockQueue.h"

constexpr key_t TEST_KEY = 0x1001;

TEST(Semaphore, LockUnlockSingleProcess) {
    auto mq = MockQueue<Message>();
    auto log = Logger(MessageLevel::INFO, &mq);
    auto sem = Semaphore(TEST_KEY, log);
}
