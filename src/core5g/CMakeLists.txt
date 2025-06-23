cmake_minimum_required(VERSION 3.10)

# Add subdirectories for each module
add_subdirectory(event_system)
add_subdirectory(mailbox) 
add_subdirectory(memory)
add_subdirectory(actor)
add_subdirectory(runtime)

# Core5G library containing common components
add_library(core5g STATIC
    core5g.h
    event_utils.h
    amf.c
    amf.h
)

target_include_directories(core5g PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core5g
    event_system
    mailbox
    memory
)

target_compile_options(core5g PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)

# Main runtime executable is built in the runtime subdirectory