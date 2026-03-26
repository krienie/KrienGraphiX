
set(EXE_NAME KGToolbox)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

if (MSVC)
add_executable(${EXE_NAME} ${Src})
elseif(APPLE)
add_executable(${EXE_NAME} MACOSX_BUNDLE ${Src})

set_target_properties(${EXE_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.kriengraphix.${EXE_NAME}"
        MACOSX_BUNDLE_BUNDLE_NAME "${EXE_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
)
endif()

target_link_libraries(${EXE_NAME} PRIVATE kriengraphix SDL3::SDL3)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

if(MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
elseif(APPLE)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
endif()
