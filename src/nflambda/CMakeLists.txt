cmake_minimum_required(VERSION 3.10)

# Add subdirectories for each module
add_subdirectory(event_system)
add_subdirectory(mailbox) 
add_subdirectory(memory)
add_subdirectory(runtime)
add_subdirectory(app)

# NFLambda library - Event-driven runtime framework
add_library(nflambda STATIC
    nflambda.c
    nflambda.h
    event_utils.h
)

target_include_directories(nflambda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(nflambda
    event_system
    mailbox
    memory
)

target_compile_options(nflambda PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)

# Main runtime executable is built in the runtime subdirectory