//
// Created by mateusz on 28.12.2025.
//

#include "services/DelivererService.h"

#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "logger/MockQueue.h"
#include "objects/Message.h"

#include "../utils/test_name.h"

#define TICK 10*1000

TEST(DelivererService, CRUD) {
    MockQueue<Message> msq;
    Logger logger(DEBUG, &msq);

    ItemTemplate tpl("a", 1, 10);
    Warehouse dst(test_name(), 1, &logger);

    DelivererService deliverers(logger, true);

    // create one deliverer
    {
        const auto created = deliverers.create("one", tpl, dst);
        ASSERT_NE(nullptr, created);
        ASSERT_EQ("one", created->name);
        ASSERT_EQ(dst.name(), created->dst_name);
        ASSERT_EQ("a", created->tpl.get().name());
        ASSERT_EQ(1, created->tpl.get().size());
        ASSERT_EQ(CREATED, created->stats->state);
        ASSERT_EQ(0, created->stats->loops);

        usleep(5*TICK);
        ASSERT_EQ(RUNNING, created->stats->state);
    }

    // create another deliverer
    {
        const auto created = deliverers.create("another", tpl, dst);
        ASSERT_NE(nullptr, created);
        ASSERT_EQ("another", created->name);
        ASSERT_EQ(dst.name(), created->dst_name);
        ASSERT_EQ("a", created->tpl.get().name());
        ASSERT_EQ(1, created->tpl.get().size());
        ASSERT_EQ(CREATED, created->stats->state);
        ASSERT_EQ(0, created->stats->loops);

        usleep(5*TICK);
        ASSERT_EQ(RUNNING, created->stats->state);
    }

    // get both
    {
        const auto both = deliverers.get_all();
        ASSERT_EQ(2, both.size());

        const auto one = both[1];
        const auto another = both[0];

        ASSERT_EQ("one", one->name);
        ASSERT_EQ(dst.name(), one->dst_name);
        ASSERT_EQ("a", one->tpl.get().name());
        ASSERT_EQ(1, one->tpl.get().size());

        ASSERT_EQ("another", another->name);
        ASSERT_EQ(dst.name(), another->dst_name);
        ASSERT_EQ("a", another->tpl.get().name());
        ASSERT_EQ(1, another->tpl.get().size());
    }

    // get one
    {
        const auto one = deliverers.get("one");

        ASSERT_NE(nullptr, one);
        ASSERT_EQ("one", one->name);
        ASSERT_EQ(dst.name(), one->dst_name);
        ASSERT_EQ("a", one->tpl.get().name());
        ASSERT_EQ(1, one->tpl.get().size());
    }
}

TEST(DelivererService, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    ItemTemplate tpl("a", 1, 100);
    Warehouse destination(test_name(), 2, &log);

    DelivererService deliverers(log, true);
    auto d = deliverers.create("test", tpl, destination);

    // initialize with empty proc.stats()!
    ASSERT_EQ(CREATED, d->stats->state);
    ASSERT_EQ(0, d->stats->loops);
    ASSERT_EQ(0, d->stats->reloads);

    usleep(3*TICK);
    ASSERT_EQ(RUNNING, d->stats->state);
    ASSERT_EQ(0, d->stats->loops);

    deliverers.pause("test");
    while (d->stats->loops == 0) {
        usleep(TICK);
    }
    ASSERT_EQ(PAUSED, d->stats->state);
    ASSERT_EQ(1, d->stats->loops);

    deliverers.resume("test");
    usleep(3*TICK);
    ASSERT_EQ(RUNNING, d->stats->state);
    ASSERT_EQ(1, d->stats->loops);

    deliverers.reload("test");
    while (d->stats->loops == 1) {
        usleep(TICK);
    }
    ASSERT_EQ(RELOADING, d->stats->state);
    ASSERT_EQ(2, d->stats->loops);

    deliverers.destroy("test");
    ASSERT_EQ(STOPPED, d->stats->state);
}
