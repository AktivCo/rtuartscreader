cmake_minimum_required (VERSION 3.0)

option(RTUARTSCREADER_BUILD_UNITTESTS "Build unittests" ON)
option(RTUARTSCREADER_RUN_UNITTESTS
       "Run unittests during build (will run if host platform is the same as target)" ON)

set(RTUARTSCREADER_USE_PIGPIO FALSE)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
	set(RTUARTSCREADER_USE_PIGPIO TRUE)
endif()

add_subdirectory(PCSC)
add_subdirectory(log)

if (RTUARTSCREADER_USE_PIGPIO)
	add_subdirectory(pigpio)
endif()

set(INSTALL_GTEST OFF CACHE BOOL "Override option" FORCE)

add_subdirectory(3rdparty/preprocessor-boost-1.73.0)
add_subdirectory(rtuartscreader)

if (RTUARTSCREADER_BUILD_UNITTESTS)
	add_subdirectory(3rdparty/googletest-release-1.10.0)
	add_subdirectory(tests/auto/rtuartscreader)
endif()
