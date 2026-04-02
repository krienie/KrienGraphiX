
set(EXE_NAME KGToolbox)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

if (MSVC)
    add_executable(${EXE_NAME} ${Src})

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source")
elseif (APPLE)
    add_executable(${EXE_NAME} MACOSX_BUNDLE ${Src})

    set_target_properties(${EXE_NAME} PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.kriengraphix.${EXE_NAME}"
            MACOSX_BUNDLE_BUNDLE_NAME "${EXE_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")

    get_target_property(APP_SHADER_FILES kriengraphix SHADER_FILES)

    if (APP_SHADER_FILES)
        target_sources(${EXE_NAME} PRIVATE ${APP_SHADER_FILES})
        set_source_files_properties(${APP_SHADER_FILES} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/shaders"
                HEADER_FILE_ONLY TRUE)
    endif()
endif()

target_link_libraries(${EXE_NAME} PRIVATE kriengraphix SDL3::SDL3)

if (MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>)
endif()

set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
