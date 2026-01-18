# Collect all implementation files from include/
file(GLOB_RECURSE LIB_SOURCES
        ${PROJECT_SOURCE_DIR}/include/*.cpp
        ${PROJECT_SOURCE_DIR}/include/*.cxx
)

add_library(factorylib STATIC
        ${LIB_SOURCES}
        ../include/objects/SharedVector.cpp
        ../include/ui/WarehouseTable.cpp
        ../include/ui/WarehouseTable.h
        ../include/ui/DelivererTable.cpp
        ../include/ui/DelivererTable.h
        ../include/ui/WorkerTable.cpp
        ../include/ui/WorkerTable.h
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
        PUBLIC
        ftxui::screen
        ftxui::component
        ftxui::dom
)

add_executable(factory main.cpp
)

target_link_libraries(factory PRIVATE factorylib)