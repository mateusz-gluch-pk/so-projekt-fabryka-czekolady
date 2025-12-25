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
    const ProcessStats *stats = proc.stats();
    if (stats->state == CREATED) {
        proc.run();
    } else {
        proc.resume();
    }
    int loops = stats->loops;
    // run for a loop
    usleep(TICK);
    ASSERT_EQ(RUNNING, stats->state);
    ASSERT_EQ(loops, stats->loops);

    // stop immediately after delivering one item
    proc.pause();
    while (stats->loops == loops) {
        usleep(TICK);
    }
    ASSERT_EQ(PAUSED, stats->state);
    ASSERT_EQ(loops+1, stats->loops);
}


#endif //FACTORY_RUN_ONCE_H