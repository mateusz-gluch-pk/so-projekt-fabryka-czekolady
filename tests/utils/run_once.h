//
// Created by mateusz on 25.12.2025.
//

#ifndef FACTORY_RUN_ONCE_H
#define FACTORY_RUN_ONCE_H

#include <gtest/gtest.h>
#include "processes/ProcessController.h"

#define TICK 10 * 1000

inline void run_once(ProcessController &proc) {
    // run deliverer - after a while, warehouse should have a new item
    if (proc.stats().state == CREATED) {
        proc.run();
    } else {
        proc.resume();
    }
    // run for a loop
    usleep(5*TICK);
    ASSERT_EQ(RUNNING, proc.stats().state);
    ASSERT_EQ(0, proc.stats().loops);

    // stop immediately after delivering one item
    proc.pause();
    while (proc.stats().loops == 0) {
        usleep(TICK);
    }
    ASSERT_EQ(PAUSED, proc.stats().state);
    ASSERT_EQ(1, proc.stats().loops);

}


#endif //FACTORY_RUN_ONCE_H