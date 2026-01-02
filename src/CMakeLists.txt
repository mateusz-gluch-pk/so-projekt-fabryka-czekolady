# Collect all implementation files from include/
file(GLOB_RECURSE LIB_SOURCES
        ${PROJECT_SOURCE_DIR}/include/*.cpp
        ${PROJECT_SOURCE_DIR}/include/*.cxx
)
add_library(factorylib STATIC
        ${LIB_SOURCES}
        ../include/stations/WarehouseStats.h
        ../include/processes/ProcessStats.cpp
        ../include/processes/ProcessStats.h
        ../include/ipcs/key.h
)

set_target_properties(factorylib PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_include_directories(factorylib
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(factorylib
        PUBLIC nlohmann_json::nlohmann_json
)

target_link_libraries(factorylib
        PRIVATE
        ftxui::screen
        ftxui::component
        ftxui::dom
)
add_executable(factory main.cpp
        ../include/services/WarehouseService.cpp
        ../include/services/WarehouseService.h
        ../include/services/WorkerService.cpp
        ../include/services/WorkerService.h
        ../include/services/DelivererService.cpp
        ../include/services/DelivererService.h
        ../include/services/LoggerService.cpp
        ../include/services/LoggerService.h
        ../include/ui/ControlPanel.cpp
        ../include/ui/ControlPanel.h
        ../include/ui/Logs.cpp
        ../include/ui/Logs.h
        ../include/ui/Dashboard.cpp
        ../include/ui/Dashboard.h
        ../include/ui/Layout.cpp
        ../include/ui/Layout.h
)

target_link_libraries(factory PRIVATE factorylib)