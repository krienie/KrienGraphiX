
set(LIB_NAME kriengraphix)

file(GLOB PrivateSrcDirs "Private/*")
set(PrivateRHIDir "${PrivateSrcDirs}")
list(FILTER PrivateSrcDirs EXCLUDE REGEX "/RHI$")
list(FILTER PrivateRHIDir INCLUDE REGEX "/RHI$")

# All directories except for RHI
foreach(PrivateDir IN LISTS PrivateSrcDirs)
    file(GLOB PrivateSrc "${PrivateDir}/*.cpp" "${PrivateDir}/*.h")
    list(APPEND Src "${PrivateSrc}")
endforeach()

# All files directly in the RHI directory
file(GLOB RHISrc "${PrivateRHIDir}/*.cpp" "${PrivateRHIDir}/*.h")
list(APPEND Src "${RHISrc}")

# Platform specific graphics APIs
if (MSVC)
    file(GLOB_RECURSE D3D12Src "${PrivateRHIDir}/D3D12/*.cpp" "${PrivateRHIDir}/D3D12/*.h")
    list(APPEND Src "${D3D12Src}")
elseif(APPLE)
    file(GLOB_RECURSE MTLSrc "${PrivateRHIDir}/Metal/*.cpp" "${PrivateRHIDir}/Metal/*.h")
    list(APPEND Src "${MTLSrc}")
endif()

file(GLOB_RECURSE PublicSrc "Public/*")
list(APPEND Src "${PublicSrc}")

add_library(${LIB_NAME} STATIC ${Src})

find_package(spdlog REQUIRED)

target_include_directories(${LIB_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>")
target_link_libraries(${LIB_NAME} PRIVATE shadercompiler SDL3::SDL3)
target_link_libraries(${LIB_NAME} PUBLIC glm spdlog::spdlog)

if (MSVC)
    target_compile_options(${LIB_NAME} PUBLIC /GR-) # No RTTI
elseif (APPLE)
    target_compile_options(${LIB_NAME} PUBLIC -fno-rtti) # No RTTI
endif()

if (WIN32)
    target_link_libraries(${LIB_NAME} PUBLIC d3d12.lib dxguid.lib dxgi.lib D3DCompiler.lib)
elseif (APPLE)
    target_link_libraries(${LIB_NAME} PUBLIC metalcpp)

    find_library(CORE_FOUNDATION CoreFoundation)
    target_link_libraries(${LIB_NAME} PRIVATE ${CORE_FOUNDATION})
else()
    message(FATAL_ERROR "Unsupported platform detected.")
endif()

file(GLOB_RECURSE ShaderFiles "Shaders/*")

if (WIN32)
    foreach(F IN LISTS ShaderFiles)
        get_filename_component(fileName "${F}" NAME)
        configure_file(${F} ${CMAKE_BINARY_DIR}/Debug/Shaders/${fileName} COPYONLY)
        configure_file(${F} ${CMAKE_BINARY_DIR}/Release/Shaders/${fileName} COPYONLY)
    endforeach()

    # Don't try to compile any shader files
    set_property(SOURCE ${ShaderFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
elseif (APPLE)
    set_target_properties(${LIB_NAME} PROPERTIES SHADER_FILES "${ShaderFiles}")
endif()

target_sources(${LIB_NAME} PRIVATE ${ShaderFiles})

source_group(Shaders FILES ${ShaderFiles})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})
