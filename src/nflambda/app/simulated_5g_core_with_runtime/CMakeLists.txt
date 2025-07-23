# Simulated 5G Core with Runtime Application
cmake_minimum_required(VERSION 3.10)

# Define the Simulated 5G Core executable
add_executable(end_to_end_5g
    end_to_end_5g_main.c
    fiveg_core_actor.c
    ueransim_actor.c
    amf.c
    ausf.c
    udm.c
    smf.c
    upf.c
    pcf.c
    nssf.c
)

# Include directories
target_include_directories(end_to_end_5g PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link with the runtime library
target_link_libraries(end_to_end_5g
    runtime_lib
    pthread
)

# Set C standard
set_target_properties(end_to_end_5g PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)