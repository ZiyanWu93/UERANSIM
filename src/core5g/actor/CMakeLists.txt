# Actor module
add_library(amf_actor STATIC
    amf_actor.c
    amf_actor.h
)

target_include_directories(amf_actor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(amf_actor
    event_system
)