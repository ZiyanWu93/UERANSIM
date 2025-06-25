# NFLambda 5G Core Application
cmake_minimum_required(VERSION 3.10)

# NAS IPC Protocol Library
add_library(nas_ipc_protocol STATIC
    nas_ipc_protocol.c
    nas_ipc_protocol.h
)

# Include directories
target_include_directories(nas_ipc_protocol PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Set C standard
set_target_properties(nas_ipc_protocol PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Unit test executable
add_executable(nas_ipc_protocol_test
    nas_ipc_protocol_test.c
)

# Link test with protocol library
target_link_libraries(nas_ipc_protocol_test
    nas_ipc_protocol
)

# Set test properties
set_target_properties(nas_ipc_protocol_test PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Enable testing
enable_testing()
add_test(NAME nas_ipc_protocol_test COMMAND nas_ipc_protocol_test)

# NFLambda 5G Core executable
add_executable(nflambda_5gcore
    main.c
    core_5g_actor.c
    amf_handlers.c
    ue_state.c
)

# Link with NFLambda libraries
target_link_libraries(nflambda_5gcore
    runtime_lib
    event_system
    mailbox
    memory
    nas_ipc_protocol
    pthread
)

# Set executable properties
set_target_properties(nflambda_5gcore PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Test client executable
add_executable(nflambda_5gcore_test_client
    test_client.c
)

target_link_libraries(nflambda_5gcore_test_client
    nas_ipc_protocol
    event_system
)

set_target_properties(nflambda_5gcore_test_client PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)