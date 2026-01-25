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

void Supervisor::pause_deliverers() const {
    for (const auto deliverer: _deliverers.get_all()) {
        _deliverers.pause(deliverer->name);
    }
}

void Supervisor::resume_deliverers() const {
    for (const auto deliverer: _deliverers.get_all()) {
        _deliverers.resume(deliverer->name);
    }
}


void Supervisor::stop_workers() const {
    for (const auto worker : _workers.get_all()) {
        _workers.destroy(worker->name);
    }
}

void Supervisor::pause_workers() const {
    for (const auto worker : _workers.get_all()) {
        _workers.pause(worker->name);
    }
}

void Supervisor::resume_workers() const {
    for (const auto worker : _workers.get_all()) {
        _workers.resume(worker->name);
    }
}

void Supervisor::pause_warehouses() const {
    _warehouses.lock();
}

void Supervisor::resume_warehouses() const {
    _warehouses.unlock();
}

void Supervisor::stop_warehouses() const {
    _warehouses.lock();
    _workers.reload_all();
    _deliverers.reload_all();
    for (const auto &wh : _warehouses.get_all_stats()) {
        _warehouses.destroy(wh.name());
    }
    _warehouses.unlock();
}

void Supervisor::stop_workers_warehouses() const {
    _warehouses.lock();
    for (const auto worker : _workers.get_all()) {
        _workers.destroy(worker->name);
    }

    _deliverers.reload_all();
    for (const auto &wh : _warehouses.get_all_stats()) {
        _warehouses.destroy(wh.name());
    }
    _warehouses.unlock();
}

void Supervisor::stop_all() const {
    _exit.request();
}
