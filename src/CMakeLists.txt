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

add_executable(factory main.cpp)

target_link_libraries(factory PRIVATE factorylib)