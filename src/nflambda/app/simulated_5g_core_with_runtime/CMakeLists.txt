# Simulated 5G Core with Runtime Application
cmake_minimum_required(VERSION 3.10)

# Define the Simulated 5G Core executable
add_executable(simulated_5g_core_with_runtime_demo
    amf_main.c
    fiveg_core_actor.c
    ueransim_actor.c
)

# Include directories
target_include_directories(simulated_5g_core_with_runtime_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link with the runtime library
target_link_libraries(simulated_5g_core_with_runtime_demo
    runtime_lib
    pthread
)

# Set C standard
set_target_properties(simulated_5g_core_with_runtime_demo PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)