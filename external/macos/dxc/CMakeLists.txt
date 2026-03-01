
add_library(dxc SHARED IMPORTED GLOBAL)
set_target_properties(dxc PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libdxcompiler.dylib"
)

#TODO(KL): need to probably copy this to the correct location
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/libdxcompiler.dylib ${CMAKE_BINARY_DIR}/Debug/libdxcompiler.dylib COPYONLY)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/libdxcompiler.dylib ${CMAKE_BINARY_DIR}/Release/libdxcompiler.dylib COPYONLY)
