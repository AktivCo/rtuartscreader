project(rtuart_transport_test)

file(GLOB SOURCES "*.h" "*.c" "*.cpp")

if (CMAKE_VERSION VERSION_LESS "3.1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} pcsc-headers TransportAPI)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIR}/${PROJECT_NAME})

if (NOT DEFINED TRANSPORT_TEST_INSTALL_PATH)
    set(TRANSPORT_TEST_INSTALL_PATH "/usr/bin")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${TRANSPORT_TEST_INSTALL_PATH})
