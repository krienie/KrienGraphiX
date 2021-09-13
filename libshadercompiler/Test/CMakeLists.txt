
enable_testing()

set(EXE_NAME libshadercompiler.test)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

add_executable(${EXE_NAME} ${Src})

target_include_directories(${EXE_NAME} PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
target_link_libraries(${EXE_NAME}
                        PUBLIC
                            libshadercompiler
                        PRIVATE
                            CONAN_PKG::gtest)

if(MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/x64/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/Debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/Release)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/x64/Release)
elseif(MACOSX)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/x64/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/x64/release)
endif()

file(GLOB_RECURSE TestDataFiles
        "Data/*")
foreach(F IN LISTS TestDataFiles)
    get_filename_component(fileName "${F}" NAME)
    configure_file(${F} ${CMAKE_BINARY_DIR}/x64/Debug/TestData/${fileName} COPYONLY)
    configure_file(${F} ${CMAKE_BINARY_DIR}/x64/Release/TestData/${fileName} COPYONLY)
endforeach()


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})

add_test(NAME ${EXE_NAME} COMMAND ${EXE_NAME})
