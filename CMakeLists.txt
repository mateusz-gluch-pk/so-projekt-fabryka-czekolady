cmake_minimum_required(VERSION 4.0)
project(factory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# === nlohmann/json ===
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG master
)
FetchContent_MakeAvailable(nlohmann_json)

add_subdirectory(src)
add_subdirectory(tests)