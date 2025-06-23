# Event System module
add_library(event_system STATIC
    event.c
    event.h
    event_pool.c
    event_pool.h
)

target_include_directories(event_system PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Event system test executable
add_executable(event_system_test event_system_test.c)
target_link_libraries(event_system_test
    event_system
)

# Event pool test executable
add_executable(event_pool_test event_pool_test.c)
target_link_libraries(event_pool_test
    event_system
)