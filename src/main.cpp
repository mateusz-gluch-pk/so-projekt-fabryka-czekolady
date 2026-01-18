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

# define BASE_DELIVERER_DELAY 1000
// # define BASE_DELIVERER_DELAY 0


void create_log_collector(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];

    const auto log_key = make_key(LOGGING_DIR, name);
    auto msq = MessageQueue<Message>::attach(log_key);
    Logger log(INFO, &msq, log_key);

    auto proc = std::make_unique<LogCollector>(name, log);
    ProcessController::run_local(std::move(proc), log);
}

void create_deliverer(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];
    std::string dst_name = args["--dst_name"];
    std::string item_name = args["--item_name"];
    key_t log_key = static_cast<key_t>(std::stoul(args["--log_key"]));
    int dst_cap = std::stoi(args["--dst_cap"]);
    int item_delay = std::stoi(args["--item_delay"]);
    int item_size = std::stoi(args["--item_size"]);

    // auto msq = MessageQueue<Message>::attach(log_key);
    auto msq = MockQueue<Message>();
    Logger log(INFO, &msq, log_key);

    ItemTemplate t(item_name, item_size, item_delay);
    auto dst = Warehouse::attach(dst_name, dst_cap, &log);

    auto proc = std::make_unique<Deliverer>(name, t, dst, log);
    ProcessController::run_local(std::move(proc), log);
}

void create_worker(std::unordered_map<std::string, std::string> args) {
    std::string name = args["--name"];
    std::string in_name = args["--in_name"];
    int in_cap = std::stoi(args["--in_cap"]);

    std::string out_name = args["--out_name"];
    int out_cap = std::stoi(args["--out_cap"]);

    std::string output = args["--recipe_output"];
    nlohmann::json j = args["--recipe_inputs"];
    auto inputs = j.get<std::vector<Item>>();
    key_t log_key = static_cast<key_t>(std::stoul(args["--log_key"]));

    // auto msq = MessageQueue<Message>::attach(log_key);
    auto msq = MockQueue<Message>();
    Logger log(INFO, &msq, log_key);

    auto in = Warehouse::attach(in_name, in_cap, &log);
    auto out = Warehouse::attach(out_name, out_cap, &log);

    Recipe r(inputs, {output, 1, 1});

    auto proc = std::make_unique<Worker>(name, r, in, out, log);
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
    // LoggerService logger("factory-simulation", INFO);
    // Logger log = logger.get();
    auto msq = MockQueue<Message>();
    Logger log = Logger(INFO, &msq, 0);

    // Setup Services
    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);
    ShutdownToken run;

    // Supervisor
    Supervisor sv(warehouses, deliverers, workers, run, log);

    // Setup Warehouses
    // WH 1 - with capacity
    constexpr int capacity = 1000;
    auto ingredients = warehouses.create("ingredients", capacity);
    if (ingredients == nullptr) {
        return 1;
    }

    // WH 2 - with large capacity
    constexpr int output_capacity = 1000000;
    auto outputs = warehouses.create("outputs", output_capacity);
    if (outputs == nullptr) {
        return 1;
    }

    // Setup Deliverers
    // D1 - Item A
    ItemTemplate a("A", 1, BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-a", a, *ingredients);

    // D2 - Item B
    ItemTemplate b("B", 1, BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-b", b, *ingredients);

    // D3 - Item C
    ItemTemplate c("C", 2, 2*BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-c", c, *ingredients);

    // D4 - Item D
    ItemTemplate d("D", 3, 2*BASE_DELIVERER_DELAY);
    deliverers.create("deliverer-d", d, *ingredients);

    // Setup Workers
    // W1 - {A, B, C} -> T1
    std::vector<Item> r1_in{
        {"A", 1, 1},
        {"B", 1, 1},
        {"C", 2, 1},
    };
    Recipe r1(r1_in, {"T1", 1, 1});
    workers.create("worker-t1", r1, *ingredients, *outputs);

    // W2 - {A, B, D} -> T2
    std::vector<Item> r2_in{
            {"A", 1, 1},
            {"B", 1, 1},
            {"D", 3, 1},
        };
    Recipe r2(r2_in, {"T2", 1, 1});
    workers.create("worker-t2", r2, *ingredients, *outputs);

    // Setup UI
    auto control_panel = std::make_shared<ControlPanel>(sv);
    // auto log_panel = std::make_shared<LogPanel>(logger);

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
        control_panel->component()
        // log_panel->component()
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
