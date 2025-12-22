//
// Created by mateusz on 22.12.2025.
//

#include "objects/ItemTemplate.h"

#include <gtest/gtest.h>

TEST(ItemTemplate, GetItem) {
    const ItemTemplate tpl("test", 1, 100);
    const Item expected("test", 1, 1);
    const auto got = tpl.get();

    ASSERT_EQ(expected, got);
    ASSERT_EQ(expected.count(), got.count());
}

TEST(ItemTemplate, Delay) {
    const ItemTemplate tpl("test", 1, 100);
    constexpr int maxdelay = 150;
    constexpr int mindelay = 50;

    for (int i = 0; i < 100; ++i) {
        const int delay = tpl.delay_ms();
        ASSERT_GE(maxdelay, delay);
        ASSERT_LE(mindelay, delay);
    }
}
