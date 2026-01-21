//
// Created by mateusz on 22.12.2025.
//

#include "objects/Item.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TEST(Item, Getters) {
    const Item item("test", 2, 1);

    ASSERT_EQ("test", item.name());
    ASSERT_EQ(2, item.size());
    ASSERT_EQ(1, item.count());
}

TEST(Item, OperatorOverloads) {
    const Item item("test", 2, 1);

    // assignment operator
    const auto other = item;
    ASSERT_EQ("test", other.name());
    ASSERT_EQ(2, other.size());
    ASSERT_EQ(1, other.count());

    // test == operator
    ASSERT_EQ(item, other);
}

TEST(Item, StackUnstack) {
    Item a("test", 2, 3);
    Item b("test", 2, 4);

    // stacking the same item should increase stacksize to
    int stacksize = a.stack(b);
    ASSERT_EQ(7, stacksize);
    ASSERT_EQ(7, a.count());
    ASSERT_EQ(0, b.count());

    // stacking item of size 0 should be a no-op
    stacksize = a.stack(b);
    ASSERT_EQ(7, stacksize);
    ASSERT_EQ(7, a.count());
    ASSERT_EQ(0, b.count());

    // stacking other items should fail - and do nothing
    b = Item("test-other", 2, 5);
    stacksize = a.stack(b);
    ASSERT_EQ(-1, stacksize);
    ASSERT_EQ(7, a.count());
    ASSERT_EQ(5, b.count());

    b = Item("test", 3, 5);
    stacksize = a.stack(b);
    ASSERT_EQ(-1, stacksize);
    ASSERT_EQ(7, a.count());
    ASSERT_EQ(5, b.count());
}

TEST(Item, JSONSerialize) {
    std::string expected = R"({"count":1,"name":"test","size":2})";
    Item a("test", 2, 1);

    json jout = a;
    std::string actual = jout.dump();
    ASSERT_EQ(expected, actual);

    json jin = json::parse(actual);
    Item retrieved = jin.get<Item>();
    ASSERT_EQ(retrieved, a);
    ASSERT_EQ(retrieved.count(), a.count());
}
