add_executable(dlc-creator main.cpp)
target_include_directories(dlc-creator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/common)
target_link_libraries(dlc-creator z)
