cmake_minimum_required(VERSION 3.16)
project(RoboTact VERSION 1.0.0 LANGUAGES CXX C)
#-------------------------------------------------------------------------------
# CMake Policies
#-------------------------------------------------------------------------------

# Modern policy settings
cmake_policy(SET CMP0077 NEW)  # Honor BUILD_SHARED_LIBS for imported targets
cmake_policy(SET CMP0135 NEW)  # Proper handling of FetchContent dependencies

#-------------------------------------------------------------------------------
# Compiler Configuration
#-------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for macOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version")
elseif(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    if(MSVC)
        add_compile_options(/W4 /WX /permissive-)
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux settings
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-deprecated-declarations)
endif()

#-------------------------------------------------------------------------------
# Project Structure
#-------------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#-------------------------------------------------------------------------------
# Project Options
#-------------------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ROBOTACT_ENABLE_TESTS "Enable tests" OFF)
option(ROBOTACT_USE_SYSTEM_DEPS "Try to use system-installed dependencies" OFF)
option(ROBOTACT_FORCE_FETCH_DEPS "Force fetching dependencies even if system packages exist" OFF)

#-------------------------------------------------------------------------------
# Dependency Management
#-------------------------------------------------------------------------------

include(FetchContent)
include(CMakePrintHelpers)

# OpenGL is the only system dependency we absolutely need
find_package(OpenGL REQUIRED)

#-------------------------------------------------------------------------------
# GLFW
#-------------------------------------------------------------------------------

if(ROBOTACT_USE_SYSTEM_DEPS AND NOT ROBOTACT_FORCE_FETCH_DEPS)
    find_package(glfw3 3.3 QUIET)
endif()

if(NOT glfw3_FOUND)
    message(STATUS "Using FetchContent for GLFW")

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
        GIT_SHALLOW TRUE
    )

    # GLFW build options
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glfw)

    # Create consistent target name
    if(NOT TARGET glfw)
        add_library(glfw ALIAS glfw)
    endif()
endif()

#-------------------------------------------------------------------------------
# GLM (Mathematics Library)
#-------------------------------------------------------------------------------

set(GLM_FORCE_CMAKE_DEPRECATION_WARNINGS OFF CACHE BOOL "" FORCE)

# Only fetch GLM if not already available
if(NOT TARGET glm)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1  # Stable release
        GIT_SHALLOW TRUE
    )
    
    # Modern way to handle header-only libraries
    FetchContent_MakeAvailable(glm)
    
    # Create interface target if GLM doesn't create one
    if(NOT TARGET glm)
        add_library(glm INTERFACE)
        target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
    endif()
endif()

#-------------------------------------------------------------------------------
# Dear ImGui
#-------------------------------------------------------------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking  # Using docking branch
    GIT_SHALLOW TRUE
)

set(IMGUI_ENABLE_TEST_ENGINE OFF CACHE BOOL "" FORCE)
set(IMGUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(imgui)

# Create ImGui library target
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PRIVATE glfw OpenGL::GL)

#-------------------------------------------------------------------------------
# GLAD (OpenGL Loader)
#-------------------------------------------------------------------------------

# GLAD remains local as it's generated code
add_library(glad STATIC ${PROJECT_SOURCE_DIR}/dependencies/glad/src/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/glad/include)
target_link_libraries(glad PUBLIC OpenGL::GL)

# Suppress warnings in GLAD on Linux
if(UNIX AND NOT APPLE)
    target_compile_options(glad PRIVATE -w)
endif()

#-------------------------------------------------------------------------------
# STB (Single-file Libraries)
#-------------------------------------------------------------------------------

# STB remains local for version control
add_library(stb_image STATIC ${PROJECT_SOURCE_DIR}/dependencies/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/stb_image)

#-------------------------------------------------------------------------------
# Main Application
#-------------------------------------------------------------------------------

# Prefer explicit file listing over GLOB in production
file(GLOB_RECURSE PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
)

add_executable(RoboTact ${PROJECT_SOURCES})
target_include_directories(RoboTact PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
)

# Platform-specific linking
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(RoboTact PRIVATE
        ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY}
    )
endif()

target_link_libraries(RoboTact PRIVATE
    glfw
    glad
    imgui
    stb_image
    glm
    OpenGL::GL
)

#-------------------------------------------------------------------------------
# Installation and Packaging
#-------------------------------------------------------------------------------

install(TARGETS RoboTact
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bundle
)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
include(CPack)