//
// Created by mateusz on 25.12.2025.
//

#ifndef FACTORY_RUN_ONCE_H
#define FACTORY_RUN_ONCE_H

#include <gtest/gtest.h>
#include "processes/ProcessController.h"

#define TICK 10 * 1000

inline void run_once(ProcessController &proc) {
    ProcessStats stats = proc.stats();
    // run deliverer - after a while, warehouse should have a new item
    if (stats.state == CREATED) {
        proc.run();
    } else {
        proc.resume();
    }
    // run for a loop
    usleep(5*TICK);
    ASSERT_EQ(RUNNING, stats.state);
    ASSERT_EQ(0, stats.loops);

    // stop immediately after delivering one item
    proc.pause();
    while (stats.loops == 0) {
        usleep(TICK);
    }
    ASSERT_EQ(PAUSED, stats.state);
    ASSERT_EQ(1, stats.loops);
}


#endif //FACTORY_RUN_ONCE_H