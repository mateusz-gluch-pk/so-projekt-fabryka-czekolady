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
#define TEST_WORKER_DELAY 0
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

        if (proc_name == "Worker") create_worker(log, TEST_WORKER_DELAY, kv);
        return 0;
    }

    // Setup Logger
    LoggerService logger("t2", TEST_LOG_LEVEL, true);
    Logger log = logger.get();

    // Setup Services
    WarehouseService warehouses(log);
    WorkerService workers(log);

    // double warehouse
    auto ia = warehouses.create("t2a", 1);
    if (ia == nullptr) return 1;

    // preload warehouse T2A
    auto item = new_item("t2a", 1);
    ia->add(*item);

    auto ib = warehouses.create("t2b", 1);
    if (ib == nullptr) return 1;

    // Setup Worker
    std::vector<std::unique_ptr<IItem>> in;
    in.push_back(new_item("t2a", 1));
    auto r = std::make_unique<Recipe>(std::move(in), new_item("t2b", 1), TEST_WORKER_DELAY);
    auto w = workers.create("t2-worker", std::move(r), warehouses);
    if (w == nullptr) return 1;
    auto stats = w->stats;

    // wait for worker to run (single tick)
    usleep(TEST_TICK);

    // send PAUSE signal - deliverer will deliver exactly once
    workers.pause("t2-worker");
    usleep(TEST_TICK);

    // resume worker - and immediately pause (1 loop)
    workers.resume("t2-worker");
    usleep(TEST_TICK);

    workers.pause("t2-worker");
    usleep(TEST_TICK);


    log.info("Worker state: %s", state_to_string(stats->state).c_str());
    log.info("Items in warehouse A: %d", ia->usage());
    log.info("Items in warehouse B: %d", ib->usage());
    log.info("Test 2 finished");
    return 0;
}