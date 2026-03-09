
enable_testing()

find_package(GTest)

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
                            GTest::gtest)

if(MSVC)
    set_target_properties("${EXE_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
elseif(APPLE)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
    set_target_properties("${EXE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
endif()

file(GLOB_RECURSE TestDataFiles
        "Data/*")
foreach(F IN LISTS TestDataFiles)
    get_filename_component(fileName "${F}" NAME)
    configure_file(${F} ${CMAKE_BINARY_DIR}/Debug/TestData/${fileName} COPYONLY)
    configure_file(${F} ${CMAKE_BINARY_DIR}/Release/TestData/${fileName} COPYONLY)
endforeach()

target_sources(${EXE_NAME} PUBLIC ${TestDataFiles})
source_group(Shaders FILES ${TestDataFiles})
# Don't try to compile any TestData files
set_property(SOURCE ${TestDataFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})

add_test(NAME ${EXE_NAME} COMMAND ${EXE_NAME})
