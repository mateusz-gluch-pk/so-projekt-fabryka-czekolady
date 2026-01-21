//
// Created by mateusz on 22.12.2025.
//

#include "actors/Deliverer.h"

#include <gtest/gtest.h>

#include "processes/ProcessController.h"
#include "../utils/test_name.h"
#include "../utils/run_once.h"

#define TICK 1000

TEST(Deliverer, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination(test_name(), 2, &log);

    auto deliverer = std::make_unique<Deliverer>("test", tpl, destination, log);
    ProcessController proc(std::move(deliverer), log, true, true);

    const ProcessStats *stats = proc.stats();

    // initialize with empty proc.stats()!
    ASSERT_EQ(CREATED, stats->state);
    ASSERT_EQ(0, stats->loops);
    ASSERT_EQ(0, stats->reloads);

    run_once(proc);

    // item should appear in warehouse
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 1), destination.items()[0]);
    ASSERT_EQ(1, destination.items()[0].count());

    // resume
    run_once(proc);

    proc.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, stats->state);
    ASSERT_EQ(2, stats->loops);
}

TEST(Deliverer, FullWarehouseDelivery) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination(test_name(), 1, &log);

    auto deliverer = std::make_unique<Deliverer>("test", tpl, destination, log);
    ProcessController proc(std::move(deliverer), log, true, true);

    const ProcessStats *stats = proc.stats();

    // initialize with empty proc.stats()!
    ASSERT_EQ(CREATED, stats->state);
    ASSERT_EQ(0, stats->loops);
    ASSERT_EQ(0, stats->reloads);

    run_once(proc);

    // item should appear in warehouse
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 1), destination.items()[0]);
    ASSERT_EQ(1, destination.items()[0].count());

    // resume
    run_once(proc);

    // warehouse should remain unchanged
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 1), destination.items()[0]);
    ASSERT_EQ(1, destination.items()[0].count());

    proc.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, stats->state);
    ASSERT_EQ(2, stats->loops);
}

TEST(Deliverer, MultiDeliverer) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination(test_name(), 2, &log);

    auto d1 = std::make_unique<Deliverer>("test-d1", tpl, destination, log);
    ProcessController pd1(std::move(d1), log, true, true);

    auto d2 = std::make_unique<Deliverer>("test-d2", tpl, destination, log);
    ProcessController pd2(std::move(d2), log, true, true);

    const ProcessStats *sd1 = pd1.stats();
    const ProcessStats *sd2 = pd2.stats();
    run_once(pd1, pd2);

    // items should appear in warehouse
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 2), destination.items()[0]);
    ASSERT_EQ(2, destination.items()[0].count());

    // stop processes
    // if this is skipped - processes are killed with SIGKILL
    pd1.stop();
    pd2.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, sd1->state);
    ASSERT_EQ(1, sd1->loops);

    ASSERT_EQ(STOPPED, sd2->state);
    ASSERT_EQ(1, sd2->loops);
}

