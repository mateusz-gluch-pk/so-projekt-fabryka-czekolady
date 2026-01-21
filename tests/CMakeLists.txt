add_executable(test_deliverer
        Deliverer.cpp
)

target_link_libraries(test_deliverer
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

add_executable(test_stop_deliverers
        StopDeliverers.cpp
)

target_link_libraries(test_stop_deliverers
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

add_executable(test_worker
        Worker.cpp
)

target_link_libraries(test_worker
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

add_executable(test_stop_workers
        StopWorkers.cpp
)

target_link_libraries(test_stop_workers
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

add_executable(test_stop_warehouses
        StopWarehouses.cpp
)

target_link_libraries(test_stop_warehouses
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

add_executable(test_stop_chained
        StopChained.cpp
)

target_link_libraries(test_stop_chained
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

