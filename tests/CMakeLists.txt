add_executable(tests
        ipcs/MessageQueue.cpp
        ipcs/Semaphore.cpp
        ipcs/SharedMemory.cpp
        stations/Warehouse.cpp
        stations/Workstation.cpp
        actors/Worker.cpp
        actors/Deliverer.cpp
        actors/LogCollector.cpp
        objects/Item.cpp
        objects/ItemTemplate.cpp
        objects/Recipe.cpp
        objects/SharedVector.cpp
        utils/run_once.h
        utils/test_name.h
        Supervisor.cpp
        services/WarehouseService.cpp
        services/WorkerService.cpp
        services/DelivererService.cpp
)

target_link_libraries(tests
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

include(GoogleTest)

gtest_discover_tests(tests
        PROPERTIES
        RUN_SERIAL TRUE
)