PROJECT(KrienGraphiXTestApp LANGUAGES CXX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Quick Qml Widgets QuickControls2 Concurrent)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Main executable.
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE Src
        "*.cpp"
        "*.h"
        )
        
file(GLOB_RECURSE uiSrc
        ".ui")

add_executable(KrienGraphiXTestApp WIN32 ${Src} ${uiSrc})

target_link_libraries(KrienGraphiXTestApp
                                PRIVATE
                                    Qt5::Gui Qt5::Qml Qt5::Core Qt5::Quick Qt5::Widgets Qt5::QuickControls2 Qt5::Concurrent
                                    libkriengraphix
                            )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Src})

setup_msvc_debug_environment_exe()
