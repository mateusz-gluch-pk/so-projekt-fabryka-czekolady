//
// Created by mateusz on 28.12.2025.
//

//
// Created by mateusz on 28.12.2025.
//

#include "services/WorkerService.h"

#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "logger/MockQueue.h"
#include "objects/Message.h"

#include "../utils/test_name.h"

#define TICK 10*1000

TEST(WorkerService, CRUD) {
    MockQueue<Message> msq;
    Logger logger(DEBUG, &msq);

    Warehouse in(test_name(), 1, &logger);
    Warehouse out(test_name(), 2, &logger);
    std::vector<Item> recipe_input;
    recipe_input.emplace_back("a", 1, 1);
    Recipe r(recipe_input, Item("b", 1, 1));

    WorkerService workers(logger, true);

    // create one deliverer
    {
        const auto created = workers.create("one", r, in, out);
        ASSERT_NE(nullptr, created);
        ASSERT_EQ("one", created->name);
        ASSERT_EQ(in.name(), created->in_name);
        ASSERT_EQ(out.name(), created->out_name);
        ASSERT_EQ(CREATED, created->stats->state);
        ASSERT_EQ(0, created->stats->loops);

        usleep(5*TICK);
        ASSERT_EQ(RUNNING, created->stats->state);
    }

    // create another deliverer
    {
        const auto created = workers.create("another", r, in, out);
        ASSERT_NE(nullptr, created);
        ASSERT_EQ("another", created->name);
        ASSERT_EQ(in.name(), created->in_name);
        ASSERT_EQ(out.name(), created->out_name);
        ASSERT_EQ(CREATED, created->stats->state);
        ASSERT_EQ(0, created->stats->loops);

        usleep(5*TICK);
        ASSERT_EQ(RUNNING, created->stats->state);
    }

    // get both
    {
        const auto both = workers.get_all();
        ASSERT_EQ(2, both.size());

        const auto one = both[1];
        const auto another = both[0];

        ASSERT_EQ("one", one->name);
        ASSERT_EQ(in.name(), one->in_name);
        ASSERT_EQ(out.name(), one->out_name);

        ASSERT_EQ("another", another->name);
        ASSERT_EQ(in.name(), another->in_name);
        ASSERT_EQ(out.name(), another->out_name);
    }

    // get one
    {
        const auto one = workers.get("one");
        ASSERT_NE(nullptr, one);
        ASSERT_EQ("one", one->name);
        ASSERT_EQ(in.name(), one->in_name);
        ASSERT_EQ(out.name(), one->out_name);
    }
}

TEST(WorkerService, ProcessControl) {
    MockQueue<Message> msq;
    Logger logger(DEBUG, &msq);

    Warehouse in(test_name(), 1, &logger);
    Warehouse out(test_name(), 2, &logger);
    std::vector<Item> recipe_input;
    recipe_input.emplace_back("a", 1, 1);
    Recipe r(recipe_input, Item("b", 1, 1));

    WorkerService workers(logger, true);
    const auto w = workers.create("test", r, in, out);

    // initialize with empty proc.stats()!
    ASSERT_EQ(CREATED, w->stats->state);
    ASSERT_EQ(0, w->stats->loops);
    ASSERT_EQ(0, w->stats->reloads);

    usleep(TICK);
    ASSERT_EQ(RUNNING, w->stats->state);
    ASSERT_EQ(0, w->stats->loops);

    workers.pause("test");
    while (w->stats->loops == 0) {
        usleep(TICK);
    }
    ASSERT_EQ(PAUSED, w->stats->state);
    ASSERT_EQ(1, w->stats->loops);

    workers.resume("test");
    usleep(TICK);
    ASSERT_EQ(RUNNING, w->stats->state);
    ASSERT_EQ(1, w->stats->loops);

    workers.reload("test");
    while (w->stats->loops == 1) {
        usleep(TICK);
    }
    ASSERT_EQ(1, w->stats->reloads);

    workers.destroy("test");
    ASSERT_EQ(STOPPED, w->stats->state);
}
