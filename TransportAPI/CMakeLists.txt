project(TransportAPI)

set(TRANSPORT_API_PUBLIC_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory("src/api")
add_subdirectory("src/tests")