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
        main.cpp

        include/stations/Warehouse.cpp
        include/stations/Warehouse.h
        include/stations/Workstation.cpp
        include/stations/Workstation.h

        include/objects/Item.cpp
        include/objects/Item.h
        include/objects/SharedVector.h
        include/objects/ItemTemplate.cpp
        include/objects/ItemTemplate.h
        include/objects/Recipe.cpp
        include/objects/Recipe.h
        include/objects/Message.cpp
        include/objects/Message.h

        include/ipcs/Semaphore.cpp
        include/ipcs/Semaphore.h
        include/ipcs/SharedMemory.h
        include/ipcs/MessageQueue.cpp
        include/ipcs/MessageQueue.h

        include/processes/ProcessController.cpp
        include/processes/ProcessController.h

        include/actors/IRunnable.h
        include/actors/Supervisor.cpp
        include/actors/Supervisor.h
        include/actors/Worker.cpp
        include/actors/Worker.h
        include/actors/LogCollector.cpp
        include/actors/LogCollector.h
        include/actors/Deliverer.cpp
        include/actors/Deliverer.h
        include/logger/Logger.cpp
        include/logger/Logger.h
)

target_link_libraries(projekt PRIVATE nlohmann_json::nlohmann_json)
