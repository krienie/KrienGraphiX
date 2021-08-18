
add_library(dxc SHARED IMPORTED GLOBAL)
set_target_properties(dxc PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/inc"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/dxcompiler.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/dxcompiler.lib"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/dxcompiler.dll ${CMAKE_BINARY_DIR}/x64/Debug/dxcompiler.dll COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/dxcompiler.dll ${CMAKE_BINARY_DIR}/x64/Release/dxcompiler.dll COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/dxil.dll ${CMAKE_BINARY_DIR}/x64/Debug/dxil.dll COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/dxil.dll ${CMAKE_BINARY_DIR}/x64/Release/dxil.dll COPYONLY)
