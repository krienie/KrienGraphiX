
cmake_minimum_required(VERSION 3.18)

include(cmake/functions.cmake)

project(KrienGraphiX CXX)

set(CMAKE_CXX_STANDARD 20)
add_definitions(-D_UNICODE -DUNICODE)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 

if (MSVC)
    # Remove exceptions an RTTI flags from the compile options and add the no-versions of them
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    #string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    add_compile_options(/EHsc-) # No exceptions
    #add_compile_options(/GR-) # No RTTI

    #Visual studio shouldn't introduce macros for min/max
    add_definitions(-DNOMINMAX)

    add_compile_options(/MP /W3 /WX)
    add_compile_options(/wd4100) # unused parameter
    add_link_options(/IGNORE:4099) # PDB 'filename' was not found with 'object/library' or at 'path'; linking object as if no debug info
else()
    message( FATAL_ERROR "Only MSVC is supported." )
endif()

add_subdirectory(external)
add_subdirectory(libshadercompiler)
add_subdirectory(shadercompiler)
add_subdirectory(libkriengraphix)
add_subdirectory(KGToolbox)
