//
// Created by mateusz on 21.01.2026.
//

#ifndef FACTORY_RUNNERS_H
#define FACTORY_RUNNERS_H


#include "ProcessController.h"
#include "actors/Worker.h"
#include "objects/Message.h"
#include "services/WarehouseService.h"

inline void create_worker(Logger &log, int delay_ms, std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];
    WarehouseService svc(log);

    if (const auto out = svc.attach(args["--output_name"], 1); out == nullptr) {
        log.fatal("Cannot attach to output warehouse");
    }

    auto inputs = std::vector<std::unique_ptr<IItem>>();
    auto names = std::vector<std::string>();
    auto sizes = std::vector<int>();
    for (auto&& part : args["--input_names"] | std::views::split(',')) {
        names.emplace_back(part.begin(), part.end());
    }
    for (auto&& part : args["--input_sizes"] | std::views::split(',')) {
        sizes.emplace_back(std::stoi(std::string(part.begin(), part.end())));
    }
    for (int i=0; i < names.size(); i++) {
        if (const auto in = svc.attach(names[i], sizes[i]); in == nullptr) {
            log.fatal("Cannot attach to input warehouse");
        }
        inputs.emplace_back(new_item(names[i], sizes[i]));
    }

    auto r = std::make_unique<Recipe>(std::move(inputs), new_item(args["--output_name"], 1), delay_ms);
    auto proc = std::make_unique<Worker>(name, std::move(r), svc, log, true);
    ProcessController::run_local(std::move(proc), log);
}

inline void create_deliverer(Logger &log, std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];
    std::string item_name = args["--item_name"];
    int item_delay = std::stoi(args["--item_delay"]);
    int item_size = std::stoi(args["--item_size"]);

    WarehouseService svc(log);

    if (const auto out = svc.attach(item_name, item_size); out == nullptr) {
        log.fatal("Cannot attach to output warehouse");
    }

    ItemTemplate t(item_name, item_size, item_delay);
    auto proc = std::make_unique<Deliverer>(name, t, svc, log, true);
    ProcessController::run_local(std::move(proc), log);
}

inline void create_log_collector(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];
    const int tty = std::stoi(args["--tty"]);

    const auto log_key = make_key(LOGGING_DIR, name);
    // auto msq = MessageQueue<Message>::attach(log_key);
    auto msq = MockQueue<Message>();
    Logger log(INFO, &msq, log_key);

    auto proc = std::make_unique<LogCollector>(name, log, tty == 1);
    ProcessController::run_local(std::move(proc), log);
}

#endif //FACTORY_RUNNERS_H