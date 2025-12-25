//
// Created by mateusz on 22.12.2025.
//

#include "actors/Deliverer.h"

#include <gtest/gtest.h>

#include "processes/ProcessController.h"
#include "../utils/test_name.h"
#include "../utils/run_once.h"

#define TICK 10 * 1000

TEST(Deliverer, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination(test_name(), 2, &log);

    auto deliverer = std::make_unique<Deliverer>("test", tpl, destination, log);
    ProcessController proc(std::move(deliverer), log, true, true);

    ProcessStats stats = proc.stats();

    // initialize with empty stats!
    ASSERT_EQ(CREATED, stats.state);
    ASSERT_EQ(0, stats.loops);
    ASSERT_EQ(0, stats.reloads);

    run_once(proc);

    // item should appear in warehouse
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 1), destination.items()[0]);
    ASSERT_EQ(1, destination.items()[0].count());

    // resume
    run_once(proc);

    proc.stop();
    usleep(5*TICK);
    ASSERT_EQ(STOPPED, stats.state);
    ASSERT_EQ(2, stats.loops);
}

TEST(Deliverer, MultiDeliverer) {

}