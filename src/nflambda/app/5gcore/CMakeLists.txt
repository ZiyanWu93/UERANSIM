# 5G Core Application
# This is an example application built on top of NFLambda

add_library(5gcore_lib STATIC
    amf.c
    amf.h
)

target_include_directories(5gcore_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(5gcore_lib
    event_system
    memory
)