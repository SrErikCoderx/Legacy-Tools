cmake_minimum_required(VERSION 3.10)
project(Legacy-Tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Zlib
set(ZLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Ps3 Dependencies/zlib")
include_directories(${ZLIB_ROOT})
link_directories(${ZLIB_ROOT})

add_subdirectory(tools/reader)
add_subdirectory(tools/extractor)
add_subdirectory(tools/creator)
