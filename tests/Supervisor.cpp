//
// Created by mateusz on 28.12.2025.
//

#include <gtest/gtest.h>

#include "services/DelivererService.h"
#include "services/LoggerService.h"
#include "services/WarehouseService.h"
#include "services/WorkerService.h"
#include "utils/test_name.h"

TEST(Supervisor, StopWorkers) {
    LoggerService logger(test_name(), DEBUG);
    Logger log = logger.get();

    WarehouseService warehouses(log);
    WorkerService workers(log);
    DelivererService deliverers(log);

    auto src = warehouses.create(test_name(), 100);
    auto dst = warehouses.create(test_name(), 50);
}

TEST(Supervisor, StopDeliverers) {

}

TEST(Supervisor, StopWarehouses) {

}

TEST(Supervisor, StopAll) {

}