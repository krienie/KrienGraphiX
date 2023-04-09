
set(EXE_NAME shadercompiler)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

file(GLOB_RECURSE HlslSrc "*.hlsl")

add_executable(${EXE_NAME} ${Src} ${HlslSrc})

target_include_directories(${EXE_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
target_link_libraries(${EXE_NAME} PRIVATE libshadercompiler)

# Shader files are compiled during runtime of shadercompiler. This removes build errors of wrongly set settings on the shader files.
set_property(SOURCE ${HlslSrc} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

if(MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/release)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/x64/release)
elseif(MACOSX)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/release)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HlslSrc})
