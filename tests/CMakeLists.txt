# GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit test executable
add_executable(unit_tests
        ipc/shm_test.cpp
        ipc/sem_test.cpp
        ipc/msgq_test.cpp
)

# Link libraries
target_link_libraries(unit_tests
        PRIVATE
        gtest
        gtest_main
        your_lib
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(unit_tests)

# SysV IPC: force serial execution
set_tests_properties(unit_tests PROPERTIES RUN_SERIAL TRUE)