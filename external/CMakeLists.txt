
if (MSVC)
    add_subdirectory(windows)
elseif(APPLE)
    add_subdirectory(macos)
endif()

add_subdirectory(shared)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
