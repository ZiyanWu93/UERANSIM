# AMF Actor Application
cmake_minimum_required(VERSION 3.10)

# Define the AMF executable
add_executable(amf_demo
    amf_main.c
    fiveg_core_actor.c
    ueransim_actor.c
)

# Include directories
target_include_directories(amf_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link with the runtime library
target_link_libraries(amf_demo
    runtime_lib
    pthread
)

# Set C standard
set_target_properties(amf_demo PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)