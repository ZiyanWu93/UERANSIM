# Runtime module library
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
    amf_actor
)

# Runtime executable
add_executable(runtime runtime.c)

target_link_libraries(runtime
    core5g
    event_system
    mailbox
    memory
    amf_actor
)