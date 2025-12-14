add_executable(tests
        ipcs/MessageQueue.cpp
        ipcs/Semaphore.cpp
        ipcs/SharedMemory.cpp
)

target_link_libraries(tests
        PRIVATE
        /usr/lib/libgtest.a
        /usr/lib/libgtest_main.a
        factorylib
)

include(GoogleTest)

gtest_discover_tests(tests
        PROPERTIES
        RUN_SERIAL TRUE
)