//
// Created by mateusz on 25.12.2025.
//

#ifndef FACTORY_RUN_ONCE_H
#define FACTORY_RUN_ONCE_H

#include <gtest/gtest.h>
#include "processes/ProcessController.h"

#define TICK 1000

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
    usleep(10 * TICK);
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

inline void run_once(ProcessController &p1, ProcessController &p2) {
    // run deliverer - after a while, warehouse should have a new item
    const ProcessStats *s1 = p1.stats();
    const ProcessStats *s2 = p2.stats();

    if (s1->state == CREATED) {
        p1.run();
        p2.run();
    } else {
        p1.resume();
        p2.resume();
    }
    int loops1 = s1->loops;
    int loops2 = s2->loops;

    // run for a loop
    usleep(10*TICK);
    ASSERT_EQ(RUNNING, s1->state);
    ASSERT_EQ(loops1, s1->loops);
    ASSERT_EQ(RUNNING, s2->state);
    ASSERT_EQ(loops2, s2->loops);

    // stop immediately after delivering one item
    p1.pause();
    p2.pause();
    while (s1->loops == loops1 || s2->loops == loops2) {
        usleep(TICK);
    }

    ASSERT_EQ(PAUSED, s1->state);
    ASSERT_EQ(loops1+1, s1->loops);
    ASSERT_EQ(PAUSED, s2->state);
    ASSERT_EQ(loops2+1, s2->loops);
}

#endif //FACTORY_RUN_ONCE_H