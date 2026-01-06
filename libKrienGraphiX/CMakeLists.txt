
set(LIB_NAME libkriengraphix)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

add_library(${LIB_NAME} STATIC ${Src})

target_include_directories(${LIB_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>")
target_link_libraries(${LIB_NAME} PRIVATE libshadercompiler)

if (MSVC)
    target_compile_options(${LIB_NAME} PUBLIC /GR-) # No RTTI
endif()

if (WIN32)
    target_link_libraries(${LIB_NAME} PUBLIC d3d12.lib dxguid.lib dxgi.lib D3DCompiler.lib)
else()
    # TODO(KL): other platforms
    message(FATAL_ERROR "Unsupported platform detected.")
endif()

file(GLOB_RECURSE ShaderFiles
        "Shaders/*")
foreach(F IN LISTS ShaderFiles)
    get_filename_component(fileName "${F}" NAME)
    configure_file(${F} ${CMAKE_BINARY_DIR}/x64/Debug/Shaders/${fileName} COPYONLY)
    configure_file(${F} ${CMAKE_BINARY_DIR}/x64/Release/Shaders/${fileName} COPYONLY)
endforeach()

target_sources(${LIB_NAME} PRIVATE ${ShaderFiles})
source_group(Shaders FILES ${ShaderFiles})
# Don't try to compile any shader files
set_property(SOURCE ${ShaderFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})
