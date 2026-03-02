
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(glm)

get_target_property(SDL3_target SDL3::SDL3 ALIASED_TARGET)
if(MSVC)
    set_target_properties("${SDL3_target}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/x64/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
    set_target_properties("${SDL3_target}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/Debug)
    set_target_properties("${SDL3_target}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/Release)
elseif(APPLE)
    set_target_properties("${SDL3_target}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/debug)
    set_target_properties("${SDL3_target}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/release)
endif()

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)