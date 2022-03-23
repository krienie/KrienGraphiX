
set(EXE_NAME KGToolbox)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h")

file(GLOB_RECURSE Resources
        "*.ico"
        "*.rc")

add_executable(${EXE_NAME} WIN32 ${Src} ${Resources})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Resource Files" FILES ${Resources})
