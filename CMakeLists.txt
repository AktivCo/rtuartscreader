cmake_minimum_required (VERSION 3.0)

set(PROJECT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")

add_subdirectory(PCSC)
add_subdirectory(TransportAPI)
add_subdirectory(rtuart)
