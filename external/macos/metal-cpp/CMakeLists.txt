
set(LIB_NAME metalcpp)

add_library(${LIB_NAME} STATIC "source/MetalCPPDefinitions.cpp")

target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${LIB_NAME}
        "-framework Metal"
        "-framework MetalFX"
        "-framework Foundation"
        "-framework QuartzCore")
