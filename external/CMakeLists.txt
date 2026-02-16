
add_subdirectory(boost-1.90)
add_subdirectory(dxc)
add_subdirectory(gtest-1.17.0)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
