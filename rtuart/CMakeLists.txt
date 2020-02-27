project(rtuart)

file(GLOB SOURCES "*.[hc]")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} pcsc-headers TransportAPI)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIR}/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION "/usr/lib/pcsc/drivers/serial")
install(FILES "librtuart" DESTINATION "/etc/reader.conf.d/")

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wno-unused-parameter)
