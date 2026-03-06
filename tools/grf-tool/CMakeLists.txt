add_executable(grf-tool main.cpp)
target_include_directories(grf-tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/common)
target_link_libraries(grf-tool z)
