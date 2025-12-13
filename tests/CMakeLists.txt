add_executable(tests
        MessageQueue.cpp
)

target_link_libraries(tests
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# SysV IPC: force serial execution
set_tests_properties(unit_tests PROPERTIES RUN_SERIAL TRUE)