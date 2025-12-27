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

    Warehouse in(test_name()+"_in", 2, &log);
    Warehouse out(test_name()+"_out", 1, &log);

    Item i1("a", 1, 1);
    in.add(i1);
    Item i2("b", 1, 1);
    in.add(i2);

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

    proc.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, stats->state);
    ASSERT_EQ(3, stats->loops);
}

TEST(Worker, EmptyWarehouse) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);

    std::vector<Item> inputs;
    inputs.emplace_back("a", 1, 1);
    inputs.emplace_back("b", 1, 1);
    Recipe r(inputs, Item("c", 1, 1));

    Warehouse in(test_name()+"_in", 2, &log);
    Warehouse out(test_name()+"_out", 1, &log);

    auto worker = std::make_unique<Worker>("test", r, in, out, log);
    ProcessController proc(std::move(worker), log, true, true);

    const ProcessStats *stats = proc.stats();

    // initialize with empty stats!
    ASSERT_EQ(CREATED, stats->state);
    ASSERT_EQ(0, stats->loops);
    ASSERT_EQ(0, stats->reloads);

    run_once(proc);

    proc.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, stats->state);
    ASSERT_EQ(1, stats->loops);
}

TEST(Worker, MultiWorker) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);

    std::vector<Item> inputs;
    inputs.emplace_back("a", 1, 1);
    inputs.emplace_back("b", 1, 1);
    Recipe r(inputs, Item("c", 1, 1));

    Warehouse in(test_name()+"_in", 4, &log);
    Warehouse out(test_name()+"_out", 2, &log);

    Item i1("a", 1, 1);
    in.add(i1);
    i1 = Item("a", 1, 1);
    in.add(i1);

    Item i2("b", 1, 1);
    in.add(i2);
    i2 = Item("b", 1, 1);
    in.add(i2);

    auto w1 = std::make_unique<Worker>("test-w2", r, in, out, log);
    ProcessController pw1(std::move(w1), log, true, true);
    const ProcessStats *sw1 = pw1.stats();

    auto w2 = std::make_unique<Worker>("test-w1", r, in, out, log);
    ProcessController pw2(std::move(w2), log, true, true);
    const ProcessStats *sw2 = pw2.stats();

    run_once(pw1, pw2);
    // warehouse IN should contain only B
    ASSERT_EQ(1, in.items().size());
    ASSERT_EQ(Item("b", 1, 2), in.items()[0]);
    ASSERT_EQ(2, in.items()[0].count());

    run_once(pw1, pw2);

    // warehouse IN should be empty
    ASSERT_EQ(0, in.items().size());

    run_once(pw1, pw2);

    // workers should produce two items
    ASSERT_EQ(1, out.items().size());
    ASSERT_EQ(Item("c", 1, 2), out.items()[0]);
    ASSERT_EQ(2, out.items()[0].count());

    // stop processes
    // if this is skipped - processes are killed with SIGKILL
    pw1.stop();
    pw2.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, sw1->state);
    ASSERT_EQ(3, sw1->loops);
    ASSERT_EQ(STOPPED, sw2->state);
    ASSERT_EQ(3, sw2->loops);
}
