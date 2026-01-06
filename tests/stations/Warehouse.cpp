//
// Created by mateusz on 22.12.2025.
//

#include "stations/Warehouse.h"

#include <fstream>
#include <gtest/gtest.h>

#include "logger/MockQueue.h"

#include <cstdlib>
#include <ctime>

#include "../utils/test_name.h"

namespace fs = std::filesystem;

TEST(Warehouse, SingleProcessInit) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    const auto warehouse = Warehouse::create(test_name(), 1, &log);

    ASSERT_EQ(true, fs::is_regular_file("warehouses/test.key"));

    ASSERT_EQ(0, warehouse.items().size());
    ASSERT_EQ(1, warehouse.capacity());
    ASSERT_EQ(1024, warehouse.variety());
    ASSERT_EQ(0, warehouse.usage());
}

TEST(Warehouse, AddRemoveItem) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    auto warehouse = Warehouse::create(test_name(), 2, &log);

    // add item
    Item i("a", 1, 1);
    warehouse.add(i);
    ASSERT_EQ(1, warehouse.items().size());
    ASSERT_EQ(i, warehouse.items()[0]);
    ASSERT_EQ(1, warehouse.items()[0].count());
    ASSERT_EQ(1, warehouse.usage());
    ASSERT_EQ(0, i.count());

    // retrieve item
    Item j;
    warehouse.get("a", &j);
    ASSERT_EQ(0, warehouse.items().size());
    ASSERT_EQ(0, warehouse.usage());
    ASSERT_EQ(i, j);
    ASSERT_EQ(1, j.count());

    // exceeding capacity means soft failure (warn)
    Item k("double a", 3, 1);
    warehouse.add(k);
    ASSERT_EQ(0, warehouse.items().size());
    ASSERT_EQ(0, warehouse.usage());
    ASSERT_EQ(1, k.count());

    // // exceeding variety means hard failure (error)
    // Item l("a", 1, 1);
    // Item m("b", 1, 1);
    // warehouse.add(l);
    // warehouse.add(m);
    // ASSERT_EQ(1, warehouse.items().size());
    // ASSERT_EQ(1, warehouse.usage());
    // ASSERT_EQ(0, l.count());
    // ASSERT_EQ(1, m.count());
}

TEST(Warehouse, FileStorageSave) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);

    {
        auto warehouse = new Warehouse(test_name(), 3, &log);

        Item i("a", 1, 1);
        Item j("b", 1, 1);
        Item k("a", 1, 1);
        warehouse->add(i);
        warehouse->add(j);
        warehouse->add(k);

        delete warehouse;
        warehouse = nullptr;
    }

    ASSERT_EQ(true, fs::is_regular_file("warehouses/test.json"));

    std::ifstream f{"warehouses/test.json"};
    if (!f) {
        FAIL() << "File not found";
    }

    std::string expected = R"([{"count":2,"name":"a","size":1},{"count":1,"name":"b","size":1}])";
    std::string actual;
    f >> actual;
    ASSERT_EQ(expected, actual);
}

TEST(Warehouse, FileStorageLoad) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);

    std::string expected = R"([{"count":2,"name":"a","size":1},{"count":1,"name":"b","size":1}])";
    std::ofstream f{"warehouses/test2.json"};
    if (!f) {
        FAIL() << "File not found";
    }
    f << expected;
    f.close();

    ASSERT_EQ(true, fs::is_regular_file("warehouses/test2.json"));

    auto warehouse = Warehouse("test2", 3, &log);
    ASSERT_EQ(2, warehouse.items().size());
    ASSERT_EQ(Item("a", 1, 2), warehouse.items()[0]);
    ASSERT_EQ(2, warehouse.items()[0].count());
    ASSERT_EQ(Item("b", 1, 1), warehouse.items()[1]);
    ASSERT_EQ(1, warehouse.items()[1].count());
    ASSERT_EQ(3, warehouse.usage());
}

TEST(Warehouse, MultiProcess) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);
    std::string tname = test_name();
    auto parent = Warehouse::create(tname, 2, &log);

    pid_t pid = fork();
    if (pid == 0) {
        auto child = Warehouse(tname, 2, &log, false);
        // add item
        Item i("a", 1, 1);
        child.add(i);

        exit(0);
    }

    // retrieve item
    Item j;
    while (j.count() == 0) {
        parent.get("a", &j);
        usleep(10000);
    }

    ASSERT_EQ(0, parent.items().size());
    ASSERT_EQ(0, parent.usage());
    ASSERT_EQ(1, j.count());
    ASSERT_EQ("a", j.name());
    ASSERT_EQ(1, j.size());

    waitpid(pid, nullptr, 0);
}