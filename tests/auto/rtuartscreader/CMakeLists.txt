project(rtuartscreader_tests)

file(GLOB_RECURSE SOURCES "*.h" "*.cpp")

# Add test cpp file
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wno-unused-parameter)

# Link test executable against gtest & gtest_main
target_link_libraries(${PROJECT_NAME} gtest_main gmock rtuartscreader_static -static-libgcc -static-libstdc++)

if (APPLE)
	set(STATIC_CPP_LIBS)
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
	set(STATIC_CPP_LIBS "-static-libgcc -static-libstdc++")
endif()

target_link_libraries(${PROJECT_NAME} gtest_main rtuartscreader_static ${STATIC_CPP_LIBS})

if (DEFINED RUN_TESTS)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Run tests"
        POST_BUILD
        COMMAND ${PROJECT_BINARY_DIR}/${PROJECT_NAME}
    )
endif(DEFINED RUN_TESTS)

install(TARGETS ${PROJECT_NAME} DESTINATION "/usr/local/bin/")
