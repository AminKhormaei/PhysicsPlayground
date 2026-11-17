cmake_minimum_required(VERSION 3.14)

project(PhysicsPlayground)

# Modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 6.0)
set(RAYGUI_VERSION 5.0)

include(FetchContent)

# Find or download raylib
find_package(raylib ${RAYLIB_VERSION} QUIET)

if (NOT raylib_FOUND)
    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )

    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
endif()

# Download raygui
FetchContent_Declare(
        raygui
        GIT_REPOSITORY https://github.com/raysan5/raygui.git
        GIT_TAG ${RAYGUI_VERSION}
        GIT_SHALLOW TRUE
)

# Download PhysicsTools
FetchContent_Declare(
        PhysicsTools
        GIT_REPOSITORY https://github.com/AminKhormaei/PhysicsTools.git
        GIT_TAG main   # better: a version tag like v1.0.0
        GIT_PROGRESS TRUE
)

# Use my local clone instead of downloading from GitHub
set(FETCHCONTENT_SOURCE_DIR_PHYSICSTOOLS "E:/PhysicsTools")

FetchContent_MakeAvailable(raygui)
FetchContent_MakeAvailable(PhysicsTools)

# Project sources
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.h"
)

# Executable
add_executable(${PROJECT_NAME}
        WIN32
        main.cpp
        ${PROJECT_SOURCES}
)

# Copy resources next to the executable after every build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)

# Windows platform configuration
if(WIN32)
    enable_language(RC)

    target_sources(${PROJECT_NAME} PRIVATE
            resources/app.rc
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            dwmapi
    )

    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                LINK_FLAGS "/ENTRY:mainCRTStartup"
        )
    endif()
endif()

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Tools"
        "${raygui_SOURCE_DIR}/src"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        PhysicsTools::PhysicsTools
)

# Web Configurations
if (PLATFORM STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            SUFFIX ".html"
    )

    set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY"
    )
endif()

# macOS frameworks
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework IOKit"
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework QuartzCore"
    )
endif()
