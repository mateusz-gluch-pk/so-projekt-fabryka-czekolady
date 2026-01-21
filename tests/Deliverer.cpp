//
// Created by mateusz on 21.01.2026.
//

#include <gtest/gtest.h>
#include "actors/Deliverer.h"

#include "Supervisor.h"
#include "processes/runners.h"
#include "services/DelivererService.h"
#include "services/LoggerService.h"

#define TEST_LOG_LEVEL MessageLevel::DEBUG
#define TEST_DELIVERER_DELAY 0
#define TEST_TICK 10000 // 10 ms

int main(int argc, char **argv) {
    // Worker code
    if (argc >= 3) {
        std::unordered_map<std::string, std::string> kv;
        for (int i = 3; i + 1 < argc; i += 2) {
            kv[argv[i]] = argv[i + 1];
        }

        std::string proc_name = argv[2];
        if (proc_name == "LogCollector") {
            create_log_collector(kv);
            return 0;
        }

        const key_t log_key = static_cast<key_t>(std::stoul(kv["--log_key"]));
        auto msq = MessageQueue<Message>::attach(log_key);
        Logger log(TEST_LOG_LEVEL, &msq, log_key);

        if (proc_name == "Deliverer") create_deliverer(log, kv);
        return 0;
    }

    // Setup Logger
    LoggerService logger("t1", TEST_LOG_LEVEL, true);
    Logger log = logger.get();

    // Setup Services
    WarehouseService warehouses(log);
    DelivererService deliverers(log);

    // single warehouse
    auto ia = warehouses.create("t1", 1);
    if (ia == nullptr) return 1;

    // Setup Deliverers
    // D1 - Item A
    ItemTemplate a("t1", 1, TEST_DELIVERER_DELAY);
    auto d = deliverers.create("t1-deliverer", a, warehouses);
    if (d == nullptr) return 1;
    auto stats = d->stats;

    // wait for deliverer to run (single tick)
    usleep(TEST_TICK);

    // send PAUSE signal - deliverer will deliver exactly once
    deliverers.pause("t1-deliverer");
    while (stats->state == RUNNING) {
        usleep(TEST_TICK);
    }

    log.info("Deliverer state: %s", state_to_string(stats->state).c_str());
    log.info("Items in warehouse: %d", ia->usage());
    log.info("Test 1 finished");
    return 0;
}

