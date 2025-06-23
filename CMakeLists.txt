cmake_minimum_required(VERSION 3.17)
project(ueransim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Debug build settings
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -ggdb")

include_directories(src)

#################### SUB DIRECTORIES ####################

add_subdirectory(src/ext)
add_subdirectory(src/utils)
add_subdirectory(src/asn/asn1c)
add_subdirectory(src/asn/rrc)
add_subdirectory(src/asn/ngap)
add_subdirectory(src/lib)
add_subdirectory(src/core5g)
add_subdirectory(src/gnb)
add_subdirectory(src/ue)

#################### GNB EXECUTABLE ####################

add_executable(nr-gnb src/gnb.cpp)

target_link_libraries(nr-gnb pthread)

target_compile_options(nr-gnb PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(nr-gnb asn-rrc)
target_link_libraries(nr-gnb asn-ngap)
target_link_libraries(nr-gnb common-lib)
target_link_libraries(nr-gnb gnb)
target_link_libraries(nr-gnb core5g)

#################### UE EXECUTABLE ####################

add_executable(nr-ue src/ue.cpp)
target_link_libraries(nr-ue pthread)
target_compile_options(nr-ue PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(nr-ue common-lib)
target_link_libraries(nr-ue ue)

###################### IF BINDER ######################
add_library(devbnd SHARED src/binder.cpp)
target_compile_options(devbnd PRIVATE -D_GNU_SOURCE -Wall -Wextra)
target_link_options(devbnd PRIVATE -nostartfiles)
target_link_libraries(devbnd dl)

#################### CLI EXECUTABLE ####################
add_executable(nr-cli src/cli.cpp)
target_link_libraries(nr-cli pthread)
target_compile_options(nr-cli PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(nr-cli common-lib)

################### 5G Core EXECUTABLE ##################
# Runtime executable is built in src/core5g/runtime subdirectory

################### TEST EXECUTABLES ##################
# Test executables are built in their respective module directories