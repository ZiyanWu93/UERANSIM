# Memory module
add_library(memory STATIC
    memory_utils.c
    memory_utils.h
    pool_allocator.c
    pool_allocator.h
    allocator.h
)

target_include_directories(memory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Memory test executable
add_executable(memory_test memory_test.c)
target_link_libraries(memory_test
    memory
)