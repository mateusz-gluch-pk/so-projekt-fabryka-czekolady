//
// Created by mateusz on 22.12.2025.
//

#include "actors/Deliverer.h"

#include <gtest/gtest.h>

#include "processes/ProcessController.h"

TEST(Deliverer, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination("test-dst", 2, &log);

    auto deliverer = std::make_unique<Deliverer>("test", tpl, destination, log);
    ProcessController proc(std::move(deliverer), log);

    // run deliverer - after a while, warehouse should have a new item
    proc.run();
    // check when a loop executes

    // initialize with empty stats!
    ASSERT_EQ(CREATED, proc.stats().state);
    ASSERT_EQ(0, proc.stats().loops);
    ASSERT_EQ(0, proc.stats().reloads);

    // run for a loop
    while (proc.stats().loops == 0) {
        usleep(1000);
    }
    ASSERT_EQ(RUNNING, proc.stats().state);
    ASSERT_EQ(1, proc.stats().loops);

    // pause! - so that we can check warehouse
    proc.pause();
    ASSERT_EQ(PAUSED, proc.stats().state);
    ASSERT_EQ(1, proc.stats().loops);

    // item should appear in warehouse
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 1), destination.items()[0]);
    ASSERT_EQ(1, destination.items()[0].count());

    // resume
    proc.resume();
    ASSERT_EQ(RUNNING, proc.stats().state);
    ASSERT_EQ(1, proc.stats().loops);

    // run for another loop
    while (proc.stats().loops == 1) {
        usleep(1000);
    }
    ASSERT_EQ(RUNNING, proc.stats().state);
    ASSERT_EQ(2, proc.stats().loops);

    proc.stop();
    ASSERT_EQ(STOPPED, proc.stats().state);
    ASSERT_EQ(2, proc.stats().loops);
}

TEST(Deliverer, MultiDeliverer) {

}