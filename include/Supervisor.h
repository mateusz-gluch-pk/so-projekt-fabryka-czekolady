//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SUPERVISOR_H
#define PROJEKT_SUPERVISOR_H

#include "services/DelivererService.h"
#include "services/LoggerService.h"
#include "services/WarehouseService.h"
#include "services/WorkerService.h"

#include <atomic>

class ShutdownToken {
public:
    void request() { stop.store(true); }
    bool requested() const { return stop.load(); }

private:
    std::atomic<bool> stop{false};
};

class Supervisor {
public:
    explicit Supervisor(
        WarehouseService &warehouses,
        DelivererService &deliverers,
        WorkerService &workers,
        ShutdownToken &exit,
        Logger &log
    ):
        _log(log), _exit(exit), _warehouses(warehouses), _workers(workers), _deliverers(deliverers)
    {}

    ~Supervisor();

    void stop_deliverers() {
        auto deliverers = _deliverers.get_all();
        for (auto deliverer: deliverers) {
            deliverer->stop();
        }
    };

    void stop_workers() {
        auto workers = _workers.get_all();
        for (auto worker : workers) {
            worker->stop();
        }
    };

    void stop_warehouses() {
        auto warehouses = _warehouses.get_all();
        for (auto wh: warehouses) {
            _warehouses.destroy(wh->name());
        }
        _workers.reload_all();
        _deliverers.reload_all();
    };

    void stop_all() {
        _exit.request();
    };

private:
    Logger &_log;
    ShutdownToken &_exit;
    WarehouseService &_warehouses;
    WorkerService &_workers;
    DelivererService &_deliverers;
};

#endif //PROJEKT_SUPERVISOR_H