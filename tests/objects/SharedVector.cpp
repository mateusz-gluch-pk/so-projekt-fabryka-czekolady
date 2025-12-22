//
// Created by mateusz on 22.12.2025.
//

#include "objects/SharedVector.h"

#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

using json = nlohmann::json;

TEST(SharedVector, InitializationOperatorsPushBack) {
    SharedVector<int> vec(1);
    ASSERT_EQ(0, vec.size());

    vec.push_back(1);
    ASSERT_EQ(1, vec.size());
    ASSERT_EQ(1, vec[0]);

    vec[0] = 2;
    ASSERT_EQ(1, vec.size());
    ASSERT_EQ(2, vec[0]);

    try {
        vec[1] = 1;
    } catch (std::exception &e) {
        ASSERT_EQ(1, vec.size());
        ASSERT_EQ(2, vec[0]);
        ASSERT_EQ("index out of range", std::string(e.what()));
    }

    try {
        int x = vec[1];
    } catch (std::exception &e) {
        ASSERT_EQ(1, vec.size());
        ASSERT_EQ(2, vec[0]);
        ASSERT_EQ("index out of range", std::string(e.what()));
    }

    try {
        vec.push_back(1);
    } catch (std::exception &e) {
        ASSERT_EQ(1, vec.size());
        ASSERT_EQ(2, vec[0]);
        ASSERT_EQ("vector reached full capacity", std::string(e.what()));
    }
}

TEST(SharedVector, AutoLooping) {
    SharedVector<int> vec(5);
    for (int i = 0; i < 5; i++) {
        vec.push_back(i);
    }

    for (auto i : vec) {
        ASSERT_EQ(i, vec[i]);
    }
}


TEST(SharedVector, Erase) {
    SharedVector<int> vec(5);
    for (int i = 0; i < 5; i++) {
        vec.push_back(i);
    }

    vec.erase(&vec[2]);
    ASSERT_EQ(4, vec.size());
    ASSERT_EQ(0, vec[0]);
    ASSERT_EQ(1, vec[1]);
    ASSERT_EQ(3, vec[2]);
    ASSERT_EQ(4, vec[3]);
}

TEST(SharedVector, JSONSerialize) {
    SharedVector<int> vec(5);
    for (int i = 0; i < 5; i++) {
        vec.push_back(i);
    }

    std::string expected = "[0,1,2,3,4]";
    json jout = vec;
    std::string actual = jout.dump();
    ASSERT_EQ(expected, actual);

    json jin = json::parse(actual);
    SharedVector<int> retrieved(5);
    from_json(jin, vec);
    for (auto i : retrieved) {
        ASSERT_EQ(i, vec[i]);
    }
}



