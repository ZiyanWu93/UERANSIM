# IPC Echo Demo Application

add_executable(ipc_echo_demo
    ipc_echo_main.c
)

target_link_libraries(ipc_echo_demo
    runtime_lib
    event_system
    mailbox
    memory
)

# Include paths
target_include_directories(ipc_echo_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Copy executable to build directory after building
add_custom_command(TARGET ipc_echo_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ipc_echo_demo> ${CMAKE_SOURCE_DIR}/build/
)

# IPC Client Demo Application

add_executable(ipc_client_demo
    ipc_client_demo.c
)

target_link_libraries(ipc_client_demo
    event_system  # For IPC client library
)

# Include paths
target_include_directories(ipc_client_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Copy executable to build directory after building
add_custom_command(TARGET ipc_client_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ipc_client_demo> ${CMAKE_SOURCE_DIR}/build/
)