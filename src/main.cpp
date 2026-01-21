//
// Created by mateusz on 13.12.2025.
//

#include "Supervisor.h"
#include "services/DelivererService.h"
#include "services/LoggerService.h"
#include "services/WarehouseService.h"
#include "services/WorkerService.h"
#include "ui/ControlPanel.h"
#include "ui/Dashboard.h"
#include "ui/Layout.h"
#include "ui/LogPanel.h"

#include <ftxui/component/screen_interactive.hpp>

#include "ui/DelivererTable.h"
#include "ui/WarehouseTable.h"
#include "ui/WorkerTable.h"

#include <ranges>
#include <string_view>



// Do not, under any circumstances, run MessageLevel::DEBUG!
# define SIMULATION_LOG_LEVEL MessageLevel::INFO
# define BASE_DELIVERER_DELAY 1000
# define BASE_WORKER_DELAY 1000
// # define BASE_WORKER_DELAY 0
// # define BASE_DELIVERER_DELAY 0

void create_log_collector(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];

    const auto log_key = make_key(LOGGING_DIR, name);
    // auto msq = MessageQueue<Message>::attach(log_key);
    auto msq = MockQueue<Message>();
    Logger log(SIMULATION_LOG_LEVEL, &msq, log_key);

    auto proc = std::make_unique<LogCollector>(name, log, false);
    ProcessController::run_local(std::move(proc), log);
}

void create_deliverer(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];
    std::string item_name = args["--item_name"];
    int item_delay = std::stoi(args["--item_delay"]);
    int item_size = std::stoi(args["--item_size"]);

    key_t log_key = static_cast<key_t>(std::stoul(args["--log_key"]));
    auto msq = MessageQueue<Message>::attach(log_key);
    Logger log(SIMULATION_LOG_LEVEL, &msq, log_key);
    WarehouseService svc(log);

    if (const auto out = svc.attach(item_name, item_size); out == nullptr) {
        log.fatal("Cannot attach to output warehouse");
    }

    ItemTemplate t(item_name, item_size, item_delay);
    auto proc = std::make_unique<Deliverer>(name, t, svc, log);
    ProcessController::run_local(std::move(proc), log);
}

void create_worker(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];

    const key_t log_key = static_cast<key_t>(std::stoul(args["--log_key"]));
    auto msq = MessageQueue<Message>::attach(log_key);
    Logger log(SIMULATION_LOG_LEVEL, &msq, log_key);
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

    auto r = std::make_unique<Recipe>(std::move(inputs), new_item(args["--output_name"], 1), BASE_WORKER_DELAY);
    auto proc = std::make_unique<Worker>(name, std::move(r), svc, log);
    ProcessController::run_local(std::move(proc), log);
}

int main(int argc, char **argv) {
    // Worker code
    if (argc >= 3) {
        std::unordered_map<std::string, std::string> kv;
        for (int i = 3; i + 1 < argc; i += 2) {
            kv[argv[i]] = argv[i + 1];
        }

        std::string proc_name = argv[2];
        if (proc_name == "Worker") create_worker(kv);
        if (proc_name == "Deliverer") create_deliverer(kv);
        if (proc_name == "LogCollector") create_log_collector(kv);

        return 0;
    }

    // Supervisor code

    // Setup Logger
    LoggerService logger("factory-simulation", SIMULATION_LOG_LEVEL);
    Logger log = logger.get();
    // auto msq = MockQueue<Message>();
    // Logger log = Logger(SIMULATION_LOG_LEVEL, &msq, 0);

    // Setup Services
    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    // Supervisor
    Supervisor sv(warehouses, deliverers, workers, run, log);

    // Setup Warehouses
    // WH set 1 -- for ingredients
    if (auto ia = warehouses.create("A", 1); ia == nullptr) return 1;
    if (auto ib = warehouses.create("B", 1); ib == nullptr) return 1;
    if (auto ic = warehouses.create("C", 2); ic == nullptr) return 1;
    if (auto id = warehouses.create("D", 3); id == nullptr) return 1;

    // WH set 2 - for outputs
    if (auto ot1 = warehouses.create("T1", 1); ot1 == nullptr) return 1;
    if (auto ot2 = warehouses.create("T2", 1); ot2 == nullptr) return 1;

    // Setup Deliverers
    // D1 - Item A
    ItemTemplate a("A", 1, BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-a", a, warehouses);

    // D2 - Item B
    ItemTemplate b("B", 1, BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-b", b, warehouses);

    // D3 - Item C
    ItemTemplate c("C", 2, 2*BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-c", c, warehouses);

    // D4 - Item D
    ItemTemplate d("D", 3, 2*BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-d", d, warehouses);

    // Setup Workers
    // W1 - {A, B, C} -> T1
    std::vector<std::unique_ptr<IItem>> r1_in;
    r1_in.push_back(new_item("A", 1));
    r1_in.push_back(new_item("B", 1));
    r1_in.push_back(new_item("C", 2));
    auto r1 = std::make_unique<Recipe>(std::move(r1_in), new_item("T1", 1), BASE_WORKER_DELAY);
    workers.create("worker-t1", std::move(r1), warehouses);

    // W2 - {A, B, D} -> T2
    std::vector<std::unique_ptr<IItem>> r2_in;
    r2_in.push_back(new_item("A", 1));
    r2_in.push_back(new_item("B", 1));
    r2_in.push_back(new_item("D", 3));
    auto r2 = std::make_unique<Recipe>(std::move(r2_in), new_item("T2", 1), BASE_WORKER_DELAY);
    workers.create("worker-t2", std::move(r2), warehouses);

    // Setup UI
    auto control_panel = std::make_shared<ControlPanel>(sv);
    auto log_panel = std::make_shared<LogPanel>(logger);

    auto workers_table = std::make_shared<WorkerTable>(workers);
    auto warehouses_table = std::make_shared<WarehouseTable>(warehouses);
    auto deliverers_table = std::make_shared<DelivererTable>(deliverers);
    auto dashboard = std::make_shared<Dashboard>(
      warehouses_table->component(),
      workers_table->component(),
      deliverers_table->component()
    );

    auto layout = std::make_shared<Layout>(
        dashboard->component(),
        control_panel->component(),
        log_panel->component()
    );

    // --- Start terminal UI ---
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    std::thread refresher([&] {
        while (!run.requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            screen.PostEvent(ftxui::Event::Custom);
        }
        screen.Exit();
    });

    screen.Loop(layout->component());

    refresher.join();
    return 0;
}
