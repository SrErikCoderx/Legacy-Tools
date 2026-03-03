add_executable(dlc-extractor main.cpp)
target_include_directories(dlc-extractor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/common)
target_link_libraries(dlc-extractor z)
