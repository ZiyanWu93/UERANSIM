# Runtime module library (reusable)
add_library(runtime_lib STATIC
    runtime.c
    runtime.h
)

target_include_directories(runtime_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(runtime_lib
    event_system
    mailbox
    memory
)

# Runtime standalone executable
add_executable(runtime runtime_main.c)

target_link_libraries(runtime
    runtime_lib
)