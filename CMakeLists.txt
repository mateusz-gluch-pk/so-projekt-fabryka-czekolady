cmake_minimum_required(VERSION 4.0)
project(projekt)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/stations)

add_executable(projekt
        include/stations/warehouse.cpp
        include/stations/warehouse.h
        logging/logger.cpp
        main.cpp
        include/objects/item.cpp
        include/objects/item.h)
