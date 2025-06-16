file(GLOB_RECURSE HDR_FILES *.h)
file(GLOB_RECURSE SRC_FILES *.c)

add_library(core5g ${HDR_FILES} ${SRC_FILES})

target_compile_options(core5g PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)

