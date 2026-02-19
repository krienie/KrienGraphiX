
set(EXE_NAME KGToolbox)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

file(GLOB_RECURSE Resources
        "*.ico"
        "*.rc")

add_executable(${EXE_NAME} WIN32 ${Src} ${Resources})

target_link_libraries(${EXE_NAME} PRIVATE libkriengraphix SDL3::SDL3)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Resource Files" FILES ${Resources})

if(MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/x64/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/release)
elseif(MACOSX)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/release)
endif()
