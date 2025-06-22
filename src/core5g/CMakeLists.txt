file(GLOB_RECURSE HDR_FILES *.h)
file(GLOB_RECURSE SRC_FILES *.c)

# Remove runtime.c and all test files with main functions
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/runtime.c")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/event_pool/event_pool_test.c")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/event_system/event_pool_test.c")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/event_system/event_system_test.c")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/mailbox/mailbox_test.c")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/memory/memory_test.c")

add_library(core5g ${HDR_FILES} ${SRC_FILES})

target_compile_options(core5g PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
