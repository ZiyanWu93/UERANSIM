# Mailbox module
add_library(mailbox STATIC
    mailbox.c
    mailbox.h
)

target_include_directories(mailbox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(mailbox
    memory
)

# Mailbox test executable
add_executable(mailbox_test mailbox_test.c)
target_link_libraries(mailbox_test
    mailbox
    event_system
)