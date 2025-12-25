//
// Created by mateusz on 22.12.2025.
//


#include "actors/Worker.h"

#include <gtest/gtest.h>

#include "actors/Deliverer.h"
#include "logger/Logger.h"
#include "logger/MockQueue.h"
#include "objects/ItemTemplate.h"
#include "stations/Warehouse.h"

#include "../utils/test_name.h"
#include "../utils/run_once.h"

TEST(Worker, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);

    std::vector<Item> inputs;
    inputs.emplace_back("a", 1, 1);
    inputs.emplace_back("b", 1, 1);
    Recipe r(inputs, Item("c", 1, 1));

    Warehouse in(test_name(), 2, &log);
    Warehouse out(test_name(), 1, &log);

    auto worker = std::make_unique<Worker>("test", r, in, out, log);
    ProcessController proc(std::move(worker), log, true, true);

    const ProcessStats *stats = proc.stats();

    // initialize with empty stats!
    ASSERT_EQ(CREATED, stats->state);
    ASSERT_EQ(0, stats->loops);
    ASSERT_EQ(0, stats->reloads);

    run_once(proc);

    // worker should retrieve A from warehouse
    ASSERT_EQ(1, in.items().size());
    ASSERT_EQ(Item("b", 1, 1), in.items()[0]);
    ASSERT_EQ(1, in.items()[0].count());

    run_once(proc);

    // worker should retrieve B from warehouse
    ASSERT_EQ(0, in.items().size());

    run_once(proc);

    // worker should produce an item
    ASSERT_EQ(1, out.items().size());
    ASSERT_EQ(Item("c", 1, 1), out.items()[0]);
    ASSERT_EQ(1, out.items()[0].count());

    // resume
    run_once(proc);

    proc.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, stats->state);
    ASSERT_EQ(2, stats->loops);
}
