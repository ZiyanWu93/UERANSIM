file(GLOB_RECURSE HDR_FILES *.h)
file(GLOB_RECURSE SRC_FILES *.c)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/runtime.c")

add_library(core5g ${HDR_FILES} ${SRC_FILES})

target_compile_options(core5g PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
