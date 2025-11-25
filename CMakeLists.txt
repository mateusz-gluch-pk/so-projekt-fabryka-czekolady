cmake_minimum_required(VERSION 4.0)
project(projekt)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG master
)

FetchContent_MakeAvailable(nlohmann_json)

include_directories(include)
include_directories(include/stations)
include_directories(include/objects)

add_executable(projekt
        include/stations/warehouse.cpp
        include/stations/warehouse.h
        logging/logger.cpp
        main.cpp
        include/objects/item.cpp
        include/objects/item.h
        include/ipcs/Semaphore.cpp
        include/ipcs/Semaphore.h
        include/ipcs/SharedMemory.cpp
        include/ipcs/SharedMemory.h
        include/objects/SharedVector.cpp
        include/objects/SharedVector.h)

target_link_libraries(projekt PRIVATE nlohmann_json::nlohmann_json)
