
cmake_minimum_required(VERSION 3.18)

include(cmake/conan.cmake)
include(cmake/functions.cmake)

project(KrienGraphiX CXX)

set(CMAKE_CXX_STANDARD 20)
add_definitions(-D_UNICODE -DUNICODE)

if (MSVC)
    #Visual studio shouldn't introduce macros for min /max
    add_definitions(-DNOMINMAX)

    add_compile_options(/w34100) # See [1]
    add_compile_options(/MP /W3 /WX)
    add_compile_options(/we4018 /we4245 /we4388 /we4389) # See [2]
    add_link_options(/IGNORE:4099) # PDB 'filename' was not found with 'object/library' or at 'path'; linking object as if no debug info

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(
        -Wno-nested-anon-types
        -Wno-gnu-anonymous-struct
        -Wno-extra-semi # We have a bunch of these, don't want to fix them now.
        -Wno-gnu-zero-variadic-macro-arguments # Used by google-mock
        -Werror
        -pedantic)
else()
    add_compile_options(-Wextra -Werror -Wunused-parameter)
endif()

conan_cmake_run(REQUIRES boost/1.78.0 gtest/1.11.0
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing
                REMOTE conan-center)

add_subdirectory(dxc)
add_subdirectory(libshadercompiler)
add_subdirectory(shadercompiler)
add_subdirectory(libkriengraphix)
add_subdirectory(KGToolbox)


# Footnotes:
#
# [1]
# Make warning C4100 a /W3 level warning.
# C4100 (/W4){ `identifier' : unreferenced formal parameter
# e.g.
#     void f(int i) {} // C4100
#     ...
#     f(123);
#
#
# [2]
# Treat signed/unsigned warnings as error, as much as possible the same way as GCC.
#
# From http://docs.microsoft.com/:
# C4018 (/W3): `expression': signed/unsigned mismatch
# Does not trigger when both sides of the expression types of different (byte) size.
# (See: https://developercommunity.visualstudio.com/content/problem/335285/warning-c4018-does-not-work-when-the-sizes-of-type.html).
#
# C4388 (/W4): `expression': signed/unsigned mismatch
# (No documentation!)
#
# C4389 (/W4): `operator' : signed/unsigned mismatch
# This triggers on actual comparison operators.
#
# C4245 (/W4): `conversion': conversion from `type_1' to `type_2': signed/unsigned mismatch.
# This triggers when trying to convert a signed const with negative value to
# an unsigned.
#
# C4365 (/W4): `action': conversion from `type_1' to `type_2': signed/unsigned mismatch.
# This triggers on all signed/unsigned conversions.
# Make this a level 3 warning (or worse, error) at your own peril.  This
# triggers inside a lot of Qt functions, std library functions etc.
#
