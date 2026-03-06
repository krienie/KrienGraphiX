
set(LIB_NAME metalcpp)

add_library(${LIB_NAME} STATIC "Source/MetalCPPDefinitions.cpp")

target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${LIB_NAME}
        "-framework Metal"
        "-framework MetalFX"
        "-framework Foundation"
        "-framework QuartzCore")

#target_compile_definitions(${LIB_NAME} PUBLIC
#        -DNS_PRIVATE_IMPLEMENTATION
#        -DMTL_PRIVATE_IMPLEMENTATION
#        -DMTLFX_PRIVATE_IMPLEMENTATION
#        -DCA_PRIVATE_IMPLEMENTATION)
