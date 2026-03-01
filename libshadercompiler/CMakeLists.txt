
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost CONFIG REQUIRED COMPONENTS serialization program_options)

set(LIB_NAME libshadercompiler)

file(GLOB_RECURSE Src
        "Private/*.cpp"
		"Public/*.cpp"
        "Private/*.h"
		"Public/*.h")

add_library(${LIB_NAME} STATIC ${Src})

target_include_directories(${LIB_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>")
target_link_libraries(${LIB_NAME}
                        PUBLIC
                            dxc
                            Boost::serialization
                            Boost::program_options)

if (APPLE)
target_link_libraries(${LIB_NAME} PUBLIC MetalShaderConverter)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})

add_subdirectory(Test)
