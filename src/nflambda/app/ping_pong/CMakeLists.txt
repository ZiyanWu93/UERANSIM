# Ping-Pong Demo Application

add_executable(ping_pong_demo
    ping_pong_main.c
    ping_actor.c
    pong_actor.c
)

target_link_libraries(ping_pong_demo
    runtime_lib
    event_system
    mailbox
    memory
)

# Include paths
target_include_directories(ping_pong_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Copy executable to build directory after building
add_custom_command(TARGET ping_pong_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ping_pong_demo> ${CMAKE_SOURCE_DIR}/build/
)