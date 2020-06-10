cmake_minimum_required (VERSION 3.0)

add_subdirectory(PCSC)
add_subdirectory(log)

if (NOT DEFINED RUN_TESTS)
	add_subdirectory(pigpio)
endif()

set(INSTALL_GTEST OFF CACHE BOOL "Override option" FORCE)

add_subdirectory(3rdparty/preprocessor-boost-1.73.0)
add_subdirectory(3rdparty/googletest-release-1.10.0)
add_subdirectory(rtuartscreader)
add_subdirectory(tests/auto/rtuartscreader)
