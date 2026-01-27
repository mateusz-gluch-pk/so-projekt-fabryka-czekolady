//
// Created by mateusz on 21.01.2026.
//

//
// Created by mateusz on 21.01.2026.
//

#include <gtest/gtest.h>
#include "actors/Deliverer.h"

#include "Supervisor.h"
#include "processes/runners.h"
#include "services/DelivererService.h"
#include "services/LoggerService.h"

#define TEST_LOG_LEVEL MessageLevel::INFO

#define TEST_DELIVERER_DELAY 0
#define TEST_TICK 10000 // 10 ms

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
        if (proc_name == "Deliverer") create_deliverer(log, kv);
        return 0;
    }

    // Setup Logger
    LoggerService logger("t6", TEST_LOG_LEVEL, true);
    Logger log = logger.get();

    // Setup Services
    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    // Supervisor
    Supervisor sv(warehouses, deliverers, workers, run, log);

    // double warehouse
    auto ia = warehouses.create("t6a", 1);
    if (ia == nullptr) return 1;
    auto ib = warehouses.create("t6b", 1);
    if (ib == nullptr) return 1;

    // Setup Deliverers
    ItemTemplate a("t6a", 1, TEST_DELIVERER_DELAY);
    auto d = deliverers.create("t6-deliverer", a, warehouses);
    if (d == nullptr) return 1;

    // Setup Worker
    std::vector<std::unique_ptr<IItem>> in;
    in.push_back(new_item("t6a", 1));
    auto r = std::make_unique<Recipe>(std::move(in), new_item("t6b", 1), TEST_WORKER_DELAY);
    auto w = workers.create("t6-worker", std::move(r), warehouses);
    if (w == nullptr) return 1;

    usleep(10*TEST_TICK);

    sv.stop_workers();

    usleep(10*TEST_TICK);

    log.info(("Worker state: " + state_to_string(w->stats->state)).c_str());
    log.info(("Deliverer state: " + state_to_string(d->stats->state)).c_str());

    usleep(10*TEST_TICK);

    sv.stop_warehouses();

    usleep(10*TEST_TICK);

    log.info(("Worker state: " + state_to_string(w->stats->state)).c_str());
    log.info(("Deliverer state: " + state_to_string(d->stats->state)).c_str());
    log.info("Test 5 finished");
    return 0;
}