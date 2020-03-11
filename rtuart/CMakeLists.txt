project(rtuart)

set (RTUART_VERSION_MAJOR 1)
set (RTUART_VERSION_MINOR 0)
set (RTUART_VERSION_PATCH 0)
set (RTUART_VERSION_STRING ${RTUART_VERSION_MAJOR}.${RTUART_VERSION_MINOR}.${RTUART_VERSION_PATCH})

file(GLOB SOURCES "*.[hc]")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} pcsc-headers TransportAPI dl log)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wno-unused-parameter)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${RTUART_VERSION_STRING} SOVERSION ${RTUART_VERSION_MAJOR})

if(NOT DEFINED RTUART_SERIAL_PORT)
	set(RTUART_SERIAL_PORT "/dev/ttyS0")
endif()

configure_file(librtuart.in "${CMAKE_CURRENT_BINARY_DIR}/librtuart" NEWLINE_STYLE UNIX)

install(TARGETS ${PROJECT_NAME} DESTINATION "/usr/lib/pcsc/drivers/serial")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/librtuart" DESTINATION "/etc/reader.conf.d/")
