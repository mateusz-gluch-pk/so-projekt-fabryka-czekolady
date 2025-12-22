//
// Created by mateusz on 13.12.2025.
//

#include "SharedVector.h"

#include <nlohmann/json.hpp>

#include "Item.h"

// Force instantiation for SharedVector<Item>
// template void to_json(nlohmann::json &j, SharedVector<Item> const &vec);
// template void from_json(nlohmann::json const &j, SharedVector<Item> &vec);
