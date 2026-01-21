//
// Created by mateusz on 26.11.2025.
//

#include "Recipe.h"
#include <bits/this_thread_sleep.h>

namespace stime = std::chrono;
namespace sthr = std::this_thread;

std::string Recipe::try_produce(std::vector<std::unique_ptr<IItem>>& inputs,
                                std::unique_ptr<IItem>& output) const {

    // sleep for a tick (optional)
    if (_delay_ms > 0) {
        sthr::sleep_for(stime::milliseconds(_delay_ms));
    }

    // Check if all required inputs exist
    for (const auto& required : _inputs) {
        auto it = std::find_if(inputs.begin(), inputs.end(),
            [&](const std::unique_ptr<IItem>& item) {
                return item->name() == required->name() &&
                       item->size() == required->size();
            });

        if (it == inputs.end()) {
            return required->name(); // missing ingredient
        }
    }

    inputs.clear();
    output = _output->clone();

    return ""; // production succeeded
}
