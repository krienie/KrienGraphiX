
set(EXE_NAME KGToolbox)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

if (MSVC)
add_executable(${EXE_NAME} WIN32 ${Src})
elseif(APPLE)
add_executable(${EXE_NAME} MACOSX_BUNDLE ${Src})
endif()

target_link_libraries(${EXE_NAME} PRIVATE libkriengraphix SDL3::SDL3)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

if(MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
elseif(APPLE)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
endif()
