//
// Created by mateusz on 21.01.2026.
//

// this test verifies concurrent behaviors - so WARN

#include <gtest/gtest.h>
#include "actors/Deliverer.h"

#include "Supervisor.h"
#include "processes/runners.h"
#include "services/DelivererService.h"
#include "services/LoggerService.h"

#define TEST_LOG_LEVEL MessageLevel::INFO

#define TEST_WORKER_N 64
#define TEST_WORKER_DELAY 0
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

        if (proc_name == "Worker") create_worker(log, TEST_WORKER_DELAY, kv);
        return 0;
    }

    // Setup Logger
    LoggerService logger("t4", TEST_LOG_LEVEL, true);
    Logger log = logger.get();

    // Setup Services
    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    // Supervisor
    Supervisor sv(warehouses, deliverers, workers, run, log);

    // double warehouse
    auto ia = warehouses.create("t4a", 1);
    if (ia == nullptr) return 1;

    // preload warehouse T4A
    for (int i = 0; i < ia->capacity(); i ++) {
        auto item = new_item("t4a", 1);
        ia->add(*item);
    }

    auto ib = warehouses.create("t4b", 1);
    if (ib == nullptr) return 1;

    // Setup Workers - 64 of them
    // D1 - Item A
    for (int i = 0; i < TEST_WORKER_N; i ++) {
        std::vector<std::unique_ptr<IItem>> in;
        in.push_back(new_item("t4a", 1));
        auto r = std::make_unique<Recipe>(std::move(in), new_item("t4b", 1), TEST_WORKER_DELAY);
        auto w = workers.create("t4-worker-" + std::to_string(i), std::move(r), warehouses);
        if (w == nullptr) return 1;
    }

    while (ia->usage() > 0) {
        log.warn("Items in warehouse T4A: %d", ia->usage());
        log.warn("Items in warehouse T4B: %d", ib->usage());
        usleep(TEST_TICK);
    }
    sv.stop_workers();

    log.warn("Items in warehouse T4A: %d", ia->usage());
    log.warn("Items in warehouse T4B: %d", ib->usage());
    log.warn("Test 4 finished");
    return 0;
}
