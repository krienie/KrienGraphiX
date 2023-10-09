
set(LIB_NAME libkriengraphix)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

add_library(${LIB_NAME} STATIC ${Src})

target_include_directories(${LIB_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>")
target_link_libraries(${LIB_NAME} PRIVATE libshadercompiler)

IF (WIN32)
    target_link_libraries(${LIB_NAME} PUBLIC d3d12.lib dxguid.lib dxgi.lib D3DCompiler.lib)
ELSE()
    # TODO(KL): other platforms
    message( FATAL_ERROR "Unsupported platform detected." )
ENDIF()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})
