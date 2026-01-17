//
// Created by mateusz on 26.11.2025.
//

#include "Supervisor.h"

Supervisor::Supervisor(WarehouseService &warehouses, DelivererService &deliverers, WorkerService &workers,
    ShutdownToken &exit, Logger &log):
    _log(log), _exit(exit), _warehouses(warehouses), _workers(workers), _deliverers(deliverers) {}

void Supervisor::stop_deliverers() const {
    for (const auto deliverer: _deliverers.get_all()) {
        _deliverers.destroy(deliverer->name);
    }
}

void Supervisor::stop_workers() const {
    for (const auto worker : _workers.get_all()) {
        _workers.destroy(worker->name);
    }
}

void Supervisor::stop_warehouses() const {
    for (const auto worker : _workers.get_all()) {
        _workers.pause(worker->name);
    }
    for (const auto deliverer : _deliverers.get_all()) {
        _deliverers.pause(deliverer->name);
    }

    for (const auto wh : _warehouses.get_all()) {
        _warehouses.destroy(wh->name());
    }

    _workers.reload_all();
    _deliverers.reload_all();

    for (const auto worker : _workers.get_all()) {
        _workers.resume(worker->name);
    }
    for (const auto deliverer : _deliverers.get_all()) {
        _deliverers.resume(deliverer->name);
    }
}

void Supervisor::stop_all() const {
    _exit.request();
}
