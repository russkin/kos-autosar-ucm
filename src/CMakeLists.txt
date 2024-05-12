cmake_minimum_required (VERSION 3.12)

project (kos-autosar VERSION 0.0.1)

# Initialize CMake library for KasperskyOS SDK.
include (platform)
initialize_platform (FORCE_STATIC)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(Package)

# Tools for using NK parser.
include (platform/nk)

add_subdirectory (components)
add_subdirectory (app)
