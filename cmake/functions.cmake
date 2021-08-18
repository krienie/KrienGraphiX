function(setup_msvc_debug_environment_exe)
    set(options "")
    set(oneValueArgs TARGET_NAME DLL_PATH)
    set(multiValueArgs)
    cmake_parse_arguments(
        ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        "${ARGN}")

    if(${MSVC})
        #    Needed to run any .exe
        set(PathString ${ARG_DLL_PATH})
        STRING(REPLACE ";" "\\;" PathString "${PathString}")
        #Both are needed to run the tests properly
        set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT PATH=$(LocalDebuggerEnvironment)${PathString} )
    endif(${MSVC})
endfunction(setup_msvc_debug_environment_exe)