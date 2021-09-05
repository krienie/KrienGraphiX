
set(LIB_NAME libshadercompiler)

file(GLOB_RECURSE Src
        "Private/*.cpp"
		"Public/*.cpp"
        "Private/*.h"
		"Public/*.h")

add_library(${LIB_NAME} STATIC ${Src})

target_include_directories(${LIB_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>")
target_link_libraries(${LIB_NAME}
                        PUBLIC
                            dxc
                            CONAN_PKG::boost)

#IF (WIN32)
#    target_link_libraries(${LIB_NAME} PUBLIC d3d12.lib)
#ELSE()
#    # TODO(KL): other platforms
#ENDIF()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})

add_subdirectory(Test)
