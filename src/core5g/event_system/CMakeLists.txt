file(GLOB HDR_FILES *.h)
file(GLOB SRC_FILES *.c)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/event_system_test.c")

# Add the source files to the core5g library
# This is handled by the parent CMakeLists.txt that collects all .c files
