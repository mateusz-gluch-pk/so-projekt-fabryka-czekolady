//
// Created by mateusz on 22.12.2025.
//

#include "actors/Deliverer.h"

#include <gtest/gtest.h>

#include "processes/ProcessController.h"

#define SLEEP 10*1000

static std::string tname() {
    std::ostringstream oss;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    oss << "test" << std::rand() % 1000000;
    return oss.str();
}

TEST(Deliverer, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination(tname(), 2, &log);

    auto deliverer = std::make_unique<Deliverer>("test", tpl, destination, log);
    ProcessController proc(std::move(deliverer), log, true, true);

    // run deliverer - after a while, warehouse should have a new item
    proc.run();
    // check when a loop executes

    // initialize with empty stats!
    ASSERT_EQ(CREATED, proc.stats().state);
    ASSERT_EQ(0, proc.stats().loops);
    ASSERT_EQ(0, proc.stats().reloads);

    // run for a loop
    usleep(5*SLEEP);
    ASSERT_EQ(RUNNING, proc.stats().state);
    ASSERT_EQ(0, proc.stats().loops);

    // stop immediately after delivering one item
    proc.pause();
    while (proc.stats().loops == 0) {
        usleep(SLEEP);
    }
    ASSERT_EQ(PAUSED, proc.stats().state);
    ASSERT_EQ(1, proc.stats().loops);

    // item should appear in warehouse
    ASSERT_EQ(1, destination.items().size());
    ASSERT_EQ(Item("a", 1, 1), destination.items()[0]);
    ASSERT_EQ(1, destination.items()[0].count());

    // resume
    proc.resume();
    usleep(5*SLEEP);
    ASSERT_EQ(RUNNING, proc.stats().state);
    ASSERT_EQ(1, proc.stats().loops);

    // run for another loop
    proc.pause();
    while (proc.stats().loops == 1) {
        usleep(SLEEP);
    }
    ASSERT_EQ(PAUSED, proc.stats().state);
    ASSERT_EQ(2, proc.stats().loops);

    proc.stop();
    usleep(5*SLEEP);
    ASSERT_EQ(STOPPED, proc.stats().state);
    ASSERT_EQ(2, proc.stats().loops);
}

TEST(Deliverer, MultiDeliverer) {

}