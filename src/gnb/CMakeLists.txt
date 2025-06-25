file(GLOB_RECURSE HDR_FILES *.hpp)
file(GLOB_RECURSE SRC_FILES *.cpp)

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

