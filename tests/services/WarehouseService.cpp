//
// Created by mateusz on 28.12.2025.
//

#include "services/WarehouseService.h"

#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "logger/MockQueue.h"
#include "objects/Message.h"

#include "../utils/test_name.h"

TEST(WarehouseService, CRUD) {
    MockQueue<Message> msq;
    Logger logger(DEBUG, &msq);

    WarehouseService warehouses(logger, true);

    // create one
    {
        const auto created = warehouses.create("one", 1);
        ASSERT_NE(nullptr, created);
        ASSERT_EQ("one", created->name());
        ASSERT_EQ(0, created->items().size());
        ASSERT_EQ(1, created->capacity());
    }

    // create another
    {
        const auto created = warehouses.create("another", 1);
        ASSERT_NE(nullptr, created);
        ASSERT_EQ("another", created->name());
        ASSERT_EQ(0, created->items().size());
        ASSERT_EQ(1, created->capacity());
    }

    // get both
    {
        const auto both = warehouses.get_all();
        ASSERT_EQ(2, both.size());

        const auto one = both[1];
        const auto another = both[0];

        ASSERT_EQ("one", one->name());
        ASSERT_EQ(0, one->items().size());
        ASSERT_EQ(1, one->capacity());

        ASSERT_EQ("another", another->name());
        ASSERT_EQ(0, another->items().size());
        ASSERT_EQ(1, another->capacity());
    }

    // get one
    {
        const auto one = warehouses.get("one");
        ASSERT_EQ("one", one->name());
        ASSERT_EQ(0, one->items().size());
        ASSERT_EQ(1, one->capacity());
    }

    // delete another
    {
        warehouses.destroy("another");

        // another should be null
        const auto another = warehouses.get("another");
        ASSERT_EQ(nullptr, another);

        // and list of whs should contain only one
        const auto both = warehouses.get_all();
        ASSERT_EQ(1, both.size());

        const auto one = both[0];
        ASSERT_EQ("one", one->name());
        ASSERT_EQ(0, one->items().size());
        ASSERT_EQ(1, one->capacity());
    }
}