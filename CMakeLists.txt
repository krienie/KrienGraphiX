
cmake_minimum_required(VERSION 3.18)

project(KrienGraphiX CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

add_definitions(-D_UNICODE -DUNICODE)
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
add_compile_definitions($<$<CONFIG:Debug>:-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG>$<$<CONFIG:Release>:-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>)

if (MSVC)
    # Remove exceptions flag from the compile options and add the no-version of it
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    add_compile_options(/EHsc-) # No exceptions

    #Visual studio shouldn't introduce macros for min/max
    add_definitions(-DNOMINMAX)

    add_compile_options(/MP /W3 /WX)
    add_link_options(/IGNORE:4099) # PDB 'filename' was not found with 'object/library' or at 'path'; linking object as if no debug info
elseif (APPLE)
    add_compile_options(-fno-exceptions) # No exceptions
    add_compile_options(-fdeclspec)
    set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "26.0")
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

add_subdirectory(external)

if (MSVC)
    add_compile_options(/we4100) # unused parameter
endif()

add_subdirectory(libshadercompiler)
add_subdirectory(shadercompiler)
add_subdirectory(libkriengraphix)
add_subdirectory(KGToolbox)
