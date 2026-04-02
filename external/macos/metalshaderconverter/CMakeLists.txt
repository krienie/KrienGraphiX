set(TARGET MetalShaderConverter)

# This assumes the system has metal-shaderconverter installed. Perhaps there is a better way to do this, but for now this will do.
add_library(${TARGET} INTERFACE)
target_link_libraries(${TARGET} INTERFACE "/usr/local/lib/libmetalirconverter.dylib")
target_include_directories(${TARGET} INTERFACE
        "/usr/local/include/metal_irconverter"
        "/usr/local/include/metal_irconverter_ext"
        "/usr/local/include/metal_irconverter_runtime")
target_compile_definitions(${TARGET} INTERFACE IR_RUNTIME_METALCPP)
