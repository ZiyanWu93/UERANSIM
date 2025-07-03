file(GLOB_RECURSE HDR_FILES *.hpp)
file(GLOB_RECURSE SRC_FILES *.cpp)

# Original gnb library (without NFLambda)
add_library(gnb ${HDR_FILES} ${SRC_FILES})

target_include_directories(gnb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../nflambda/app/5gcore_without_runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../nflambda
)

target_compile_options(gnb PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)

target_link_libraries(gnb asn-ngap)
target_link_libraries(gnb asn-rrc)
target_link_libraries(gnb common-lib)
target_link_libraries(gnb 5gcore_without_runtime_lib)
target_link_libraries(gnb event_system)
target_link_libraries(gnb nas_ipc_protocol)

# NFLambda gnb library (with USE_NFLAMBDA defined)
add_library(gnb-nflambda ${HDR_FILES} ${SRC_FILES})

target_include_directories(gnb-nflambda PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../nflambda/app/5gcore_without_runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../nflambda
)

target_compile_options(gnb-nflambda PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter -DUSE_NFLAMBDA)

target_link_libraries(gnb-nflambda asn-ngap)
target_link_libraries(gnb-nflambda asn-rrc)
target_link_libraries(gnb-nflambda common-lib)
target_link_libraries(gnb-nflambda 5gcore_without_runtime_lib)
target_link_libraries(gnb-nflambda event_system)
target_link_libraries(gnb-nflambda nas_ipc_protocol)

# Direct calls gnb library (with USE_NFLAMBDA and USE_DIRECT_CALLS defined)
add_library(gnb-direct ${HDR_FILES} ${SRC_FILES})

target_include_directories(gnb-direct PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../nflambda/app/5gcore_without_runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../nflambda
)

target_compile_options(gnb-direct PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter -DUSE_NFLAMBDA -DUSE_DIRECT_CALLS)

target_link_libraries(gnb-direct asn-ngap)
target_link_libraries(gnb-direct asn-rrc)
target_link_libraries(gnb-direct common-lib)
target_link_libraries(gnb-direct 5gcore_without_runtime_lib)
target_link_libraries(gnb-direct event_system)
target_link_libraries(gnb-direct nas_ipc_protocol)

