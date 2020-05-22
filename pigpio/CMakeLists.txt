project(pigpio)

set(PIGPIO_PUBLIC_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")

file(GLOB SOURCES "*.[hc]")

file(GLOB_RECURSE PUBLIC_HEADERS "${PIGPIO_PUBLIC_HEADERS_DIR}/*.h")

add_library(${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PIGPIO_PUBLIC_HEADERS_DIR})

find_package(Threads)

target_link_libraries(pigpio ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
