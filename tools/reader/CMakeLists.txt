add_executable(dlc-reader main.cpp ../common/archive_format.cpp ../common/dlc_pack_format.cpp)
target_include_directories(dlc-reader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/common)
