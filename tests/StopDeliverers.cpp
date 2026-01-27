//
// Created by mateusz on 21.01.2026.
//

#include <gtest/gtest.h>
#include "actors/Deliverer.h"

#include "Supervisor.h"
#include "processes/runners.h"
#include "services/DelivererService.h"
#include "services/LoggerService.h"

// this test verifies concurrent behaviors - so WARN
#define TEST_LOG_LEVEL MessageLevel::INFO

#define TEST_DELIVERER_N 64
#define TEST_DELIVERER_DELAY 0
#define TEST_TICK 100 // .1 ms

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
    LoggerService logger("t3", TEST_LOG_LEVEL, true);
    Logger log = logger.get();

    // Setup Services
    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    // Supervisor
    Supervisor sv(warehouses, deliverers, workers, run, log);

    // single warehouse
    auto ia = warehouses.create("t3", 1);
    if (ia == nullptr) return 1;

    // Setup Deliverers - 64 of them
    // D1 - Item A
    for (int i = 0; i < TEST_DELIVERER_N; i ++) {
        ItemTemplate a("t3", 1, TEST_DELIVERER_DELAY);
        auto d = deliverers.create("t3-deliverer-" + std::to_string(i), a, warehouses);
        if (d == nullptr) return 1;
    }

    while (ia->usage() < ia->capacity()-1) {
        log.warn("Items in warehouse: %d/%d", ia->usage(), ia->capacity()-1);
        usleep(TEST_TICK);
    }
    sv.stop_deliverers();

    log.warn("Items in warehouse: %d", ia->usage());
    log.warn("Test 3 finished");
    return 0;
}
