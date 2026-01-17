//
// Created by mateusz on 28.12.2025.
//

#include "Supervisor.h"

#include <gtest/gtest.h>

#include "services/DelivererService.h"
#include "services/LoggerService.h"
#include "services/WarehouseService.h"
#include "services/WorkerService.h"
#include "utils/test_name.h"

#define TICK 10*1000

TEST(Supervisor, StopWorkers) {
    LoggerService logger(test_name(), INFO);
    Logger log = logger.get();

    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    Supervisor sv(warehouses, deliverers, workers, run, log);

    std::vector<Item> recipe_input;
    recipe_input.emplace_back("a", 1, 1);
    Recipe r(recipe_input, Item("b", 1, 1));

    {
        auto in = warehouses.create(test_name(), 1);
        auto out = warehouses.create(test_name(), 1);
        ASSERT_NE(nullptr, in);
        ASSERT_NE(nullptr, out);

        auto w1 = workers.create("w1", r, *in, *out);
        auto w2 = workers.create("w2", r, *in, *out);
        ASSERT_NE(nullptr, w1);
        ASSERT_NE(nullptr, w2);
    }

    usleep(10*TICK);

    sv.stop_workers();

    usleep(10*TICK);

    {
        auto w1 = workers.get("w1");
        ASSERT_NE(nullptr, w1);
        ASSERT_EQ(STOPPED, w1->stats->state);

        auto w2 = workers.get("w2");
        ASSERT_NE(nullptr, w2);
        ASSERT_EQ(STOPPED, w2->stats->state);
    }
}

TEST(Supervisor, StopDeliverers) {
    LoggerService logger(test_name(), INFO);
    Logger log = logger.get();

    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    Supervisor sv(warehouses, deliverers, workers, run, log);

    ItemTemplate t("a", 1, 100);

    {
        auto dst = warehouses.create(test_name(), 1);
        ASSERT_NE(nullptr, dst);

        auto d1 = deliverers.create("d1", t, *dst);
        ASSERT_NE(nullptr, d1);
        auto d2 = deliverers.create("d2", t, *dst);
        ASSERT_NE(nullptr, d2);
    }

    usleep(10*TICK);
    sv.stop_deliverers();
    usleep(10*TICK);

    {
        auto d1 = deliverers.get("d1");
        ASSERT_NE(nullptr, d1);
        ASSERT_EQ(STOPPED, d1->stats->state);

        auto d2 = deliverers.get("d2");
        ASSERT_NE(nullptr, d2);
        ASSERT_EQ(STOPPED, d2->stats->state);
    }
}

TEST(Supervisor, StopWarehouses) {
    LoggerService logger(test_name(), DEBUG);
    Logger log = logger.get();

    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    Supervisor sv(warehouses, deliverers, workers, run, log);

    std::vector<Item> recipe_input;
    recipe_input.emplace_back("a", 1, 1);
    Recipe r(recipe_input, Item("b", 1, 1));
    ItemTemplate t("a", 1, 50);

    std::string iname = test_name();
    std::string oname = test_name();
    {
        auto in = warehouses.create(iname, 1);
        auto out = warehouses.create(oname, 1);
        ASSERT_NE(nullptr, in);
        ASSERT_NE(nullptr, out);

        auto w1 = workers.create("w1", r, *in, *out);
        ASSERT_NE(nullptr, w1);

        auto d1 = deliverers.create("d1", t, *in);
        ASSERT_NE(nullptr, d1);
    }

    usleep(10*TICK);
    sv.stop_warehouses();
    usleep(10*TICK);

    {
        auto in = warehouses.get(iname);
        ASSERT_EQ(nullptr, in);

        auto out = warehouses.get(oname);
        ASSERT_EQ(nullptr, out);

        auto w1 = workers.get("w1");
        ASSERT_NE(nullptr, w1);
        ASSERT_EQ(PAUSED, w1->stats->state);

        auto d1 = deliverers.get("d1");
        ASSERT_NE(nullptr, d1);
        ASSERT_EQ(PAUSED, d1->stats->state);
    }
}

TEST(Supervisor, StopAll) {
    LoggerService logger(test_name(), INFO);
    Logger log = logger.get();

    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    Supervisor sv(warehouses, deliverers, workers, run, log);

    sv.stop_all();

    ASSERT_EQ(true, run.requested());
}