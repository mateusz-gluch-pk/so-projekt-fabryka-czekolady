//
// Created by mateusz on 22.12.2025.
//

#include "objects/Recipe.h"

#include <gtest/gtest.h>

#include "objects/Item.h"

TEST(Recipe, BasicRecipe) {
    std::vector<Item> recipe_inputs;
    recipe_inputs.push_back(Item("a", 1, 1));
    Item recipe_output("b", 1, 1);
    Recipe recipe(recipe_inputs, recipe_output);

    // basic case - 1 input makes 1 output
    std::vector<Item> inputs;
    Item output1;
    inputs.push_back(Item("a", 1, 1));
    std::string missing = recipe.try_produce(inputs, &output1);
    ASSERT_EQ("", missing);
    ASSERT_EQ(recipe_output, output1);
    ASSERT_EQ(0, inputs.size());

    // basic case - mismatch of inputs with recipe
    inputs.push_back(Item("c", 1, 1));
    Item output2;
    missing = recipe.try_produce(inputs, &output2);
    ASSERT_EQ("a", missing);
    ASSERT_EQ(Item(), output2);
    ASSERT_EQ(1, inputs.size());
    ASSERT_EQ(Item("c", 1, 1), inputs[0]);
}

TEST(Recipe, ComplexRecipe) {
    std::vector<Item> recipe_inputs;
    recipe_inputs.push_back(Item("a", 1, 1));
    recipe_inputs.push_back(Item("b", 1, 1));

    Item recipe_output("c", 1, 1);
    Recipe recipe(recipe_inputs, recipe_output);

    std::vector<Item> inputs;

    // complex case - missing both
    Item output1;
    std::string missing = recipe.try_produce(inputs, &output1);
    ASSERT_EQ("a", missing);
    ASSERT_EQ(Item(), output1);
    ASSERT_EQ(0, inputs.size());

    // complex case - missing one of 2
    inputs.push_back(Item("a", 1, 1));
    Item output2;
    missing = recipe.try_produce(inputs, &output2);
    ASSERT_EQ("b", missing);
    ASSERT_EQ(Item(), output2);
    ASSERT_EQ(1, inputs.size());
    ASSERT_EQ(Item("a", 1, 1), inputs[0]);

    // complex case - all present
    Item output3;
    inputs.push_back(Item("b", 1, 1));
    missing = recipe.try_produce(inputs, &output3);
    ASSERT_EQ("", missing);
    ASSERT_EQ(recipe_output, output3);
    ASSERT_EQ(0, inputs.size());
}
