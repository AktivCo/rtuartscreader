project(rtuartscreader)

set (RTUARTSCREADER_VERSION_MAJOR 1)
set (RTUARTSCREADER_VERSION_MINOR 0)
set (RTUARTSCREADER_VERSION_PATCH 0)
set (RTUARTSCREADER_VERSION_STRING ${RTUARTSCREADER_VERSION_MAJOR}.${RTUARTSCREADER_VERSION_MINOR}.${RTUARTSCREADER_VERSION_PATCH})

set(TARGET ${PROJECT_NAME})
set(STATIC_TARGET ${TARGET}_static)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE HEADERS "${INCLUDE_DIR}/*.h")

file(GLOB SOURCES "*.[hc]")
file(GLOB HARDWARE_ARMV7_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hardware/platform/armv7hf/*.c")
file(GLOB HARDWARE_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hardware/platform/test/*.c")
file(GLOB ISO7816_3_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/iso7816_3/*.c")
file(GLOB_RECURSE UTILS_SOURCES "utils/*.c")
file(GLOB_RECURSE TRANSPORT_SOURCES "transport/*.c")

set(SOURCES ${HEADERS} ${SOURCES} ${ISO7816_3_SOURCES} ${UTILS_SOURCES} ${TRANSPORT_SOURCES})

if (DEFINED RUN_TESTS)
	set(SOURCES ${SOURCES} ${HARDWARE_TEST_SOURCES})
else()
	set(SOURCES ${SOURCES} ${HARDWARE_ARMV7_SOURCES})
endif()

include_directories("${INCLUDE_DIR}")

add_library(${STATIC_TARGET} STATIC ${SOURCES})

set_property(TARGET ${STATIC_TARGET} PROPERTY C_STANDARD 99)

target_compile_options(${STATIC_TARGET} PRIVATE -Werror -Wall -Wextra -Wno-unused-parameter -fPIC)

target_include_directories(${STATIC_TARGET} PUBLIC "${INCLUDE_DIR}")

set(DEPS pcsc-headers dl log boost_preprocessor m)
if (NOT DEFINED RUN_TESTS)
	set(DEPS ${DEPS} pigpio)
endif()

target_link_libraries(${STATIC_TARGET} ${DEPS})

add_library(${TARGET} SHARED dummy.c)

set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE C)

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
	target_link_libraries(${TARGET} -Wl,-force_load ${STATIC_TARGET})
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
	target_link_libraries(${TARGET} -Wl,--whole-archive ${STATIC_TARGET} -Wl,--no-whole-archive)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif()

set_target_properties(${TARGET} PROPERTIES VERSION ${RTUARTSCREADER_VERSION_STRING} SOVERSION ${RTUARTSCREADER_VERSION_MAJOR})


if(NOT DEFINED RTUARTSCREADER_SERIAL_PORT)
	set(RTUARTSCREADER_SERIAL_PORT "/dev/ttyS0")
endif()

configure_file(librtuartscreader.in "${CMAKE_CURRENT_BINARY_DIR}/librtuartscreader" NEWLINE_STYLE UNIX)


install(TARGETS ${TARGET} DESTINATION "/usr/lib/pcsc/drivers/serial")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/librtuartscreader" DESTINATION "/etc/reader.conf.d/")
