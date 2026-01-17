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

# define BASE_DELIVERER_DELAY 0

int main() {
    // Setup Logger
    LoggerService logger("factory-simulation", INFO);
    Logger log = logger.get();

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
