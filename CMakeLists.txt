
cmake_minimum_required(VERSION 3.14)

project(PhysicsPlayground)

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

FetchContent_MakeAvailable(raygui)

# Project sources
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*"
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

# Windows resource file
if(WIN32)
    enable_language(RC)

    target_sources(${PROJECT_NAME} PRIVATE
            resources/app.rc
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Tools"
        "${raygui_SOURCE_DIR}/src"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
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
