//
// Created by mateusz on 22.12.2025.
//

#include "stations/Warehouse.h"

#include <gtest/gtest.h>

#include "logger/MockQueue.h"

namespace fs = std::filesystem;

TEST(Warehouse, SingleProcessInit) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    const auto warehouse = Warehouse::create("test", 1, &log, 1);

    ASSERT_EQ(true, fs::is_regular_file("warehouses/test.key"));

    ASSERT_EQ(0, warehouse.items().size());
    ASSERT_EQ(1, warehouse.capacity());
    ASSERT_EQ(1, warehouse.variety());
    ASSERT_EQ(0, warehouse.usage());
    ASSERT_EQ("test", warehouse.name());
}

TEST(Warehouse, AddRemoveItem) {

}

TEST(Warehouse, FileStorage) {

}

TEST(Warehouse, MultiProcessInit) {

}

TEST(Warehouse, MultiProcessEdit) {

}