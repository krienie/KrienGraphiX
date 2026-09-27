
set(LIB_NAME ufbx)

add_library(${LIB_NAME} STATIC ufbx.cpp ufbx.h)
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)