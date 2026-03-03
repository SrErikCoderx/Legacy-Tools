add_executable(dlc-creator main.cpp ../common/archive_format.cpp ../common/dlc_pack_format.cpp)
target_include_directories(dlc-creator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/common)
target_link_libraries(dlc-creator z)
