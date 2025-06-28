cmake_minimum_required(VERSION 3.16...3.27)
project(RoboTact VERSION 1.0.0 LANGUAGES CXX C)

#-------------------------------------------------------------------------------
# CMake Policies
#-------------------------------------------------------------------------------

# Modern policy settings
cmake_policy(SET CMP0077 NEW)  # Honor BUILD_SHARED_LIBS for imported targets
cmake_policy(SET CMP0135 NEW)  # Proper handling of FetchContent dependencies
#cmake_policy(SET CMP0148 NEW)  # Handle Python components properly
 
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
    set(CMAKE_MACOSX_RPATH ON)
    add_compile_definitions(ROBOTACT_PLATFORM_MACOS=1)
elseif(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(ROBOTACT_PLATFORM_WINDOWS=1)
    if(MSVC)
        add_compile_options(/W4 /WX /permissive-)
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux settings
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-deprecated-declarations)
    add_compile_definitions(ROBOTACT_PLATFORM_LINUX=1)
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
# OpenCV
#-------------------------------------------------------------------------------
if(NOT ROBOTACT_USE_SYSTEM_DEPS OR ROBOTACT_FORCE_FETCH_DEPS)
    message(STATUS "Configuring OpenCV with complete header path resolution")

    # Set OpenCV build options
    set(BUILD_LIST core imgproc imgcodecs videoio highgui CACHE STRING "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_world OFF CACHE BOOL "" FORCE)
    set(OPENCV_DISABLE_ARCH_PATH ON CACHE BOOL "" FORCE)  # Important for macOS

    # Fetch OpenCV and contrib
    FetchContent_Declare(
            opencv_contrib
            GIT_REPOSITORY https://github.com/opencv/opencv_contrib.git
            GIT_TAG 4.11.0
            GIT_SHALLOW TRUE
    )

    FetchContent_Declare(
            opencv
            GIT_REPOSITORY https://github.com/opencv/opencv.git
            GIT_TAG 4.11.0
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(opencv_contrib opencv)

    if(TARGET opencv_core)
        # Get all possible include directories
        set(OPENCV_INCLUDE_DIRS
                "${opencv_SOURCE_DIR}/include"
                "${opencv_BINARY_DIR}"
                # Module-specific includes
                "${opencv_SOURCE_DIR}/modules/core/include"
                "${opencv_SOURCE_DIR}/modules/imgproc/include"
                "${opencv_SOURCE_DIR}/modules/imgcodecs/include"
                "${opencv_SOURCE_DIR}/modules/videoio/include"
                "${opencv_SOURCE_DIR}/modules/highgui/include"
                # Possible binary locations
                "${opencv_BINARY_DIR}/modules/core"
                "${opencv_BINARY_DIR}/modules/imgproc"
                "${opencv_BINARY_DIR}/modules/imgcodecs"
        )

        # Create imported target
        add_library(OpenCV::OpenCV INTERFACE IMPORTED)
        target_include_directories(OpenCV::OpenCV INTERFACE
    ${OPENCV_INCLUDE_DIRS}
    ${opencv_BINARY_DIR}  # Add this line
)
        target_link_libraries(OpenCV::OpenCV INTERFACE
                opencv_core
                opencv_imgproc
                opencv_imgcodecs
                opencv_videoio
                opencv_highgui
        )

        # Verify critical headers
        set(REQUIRED_HEADERS
                "opencv2/core.hpp"
                "opencv2/imgproc.hpp"
                "opencv2/imgcodecs.hpp"
                "opencv2/videoio.hpp"
                "opencv2/highgui.hpp"
        )

        foreach(header IN LISTS REQUIRED_HEADERS)
            find_file(HEADER_FILE ${header} PATHS ${OPENCV_INCLUDE_DIRS} NO_DEFAULT_PATH)
            if(NOT HEADER_FILE)
                message(WARNING "OpenCV header not found: ${header}")
                # Try to find in alternative paths
                file(GLOB_RECURSE FOUND_HEADER
                        "${opencv_SOURCE_DIR}/modules/*/${header}"
                        "${opencv_BINARY_DIR}/modules/*/${header}")
                if(FOUND_HEADER)
                    get_filename_component(HEADER_DIR "${FOUND_HEADER}" DIRECTORY)
                    target_include_directories(OpenCV::OpenCV INTERFACE ${HEADER_DIR})
                    message(STATUS "Found ${header} at: ${FOUND_HEADER}")
                else()
                    message(FATAL_ERROR "Critical OpenCV header missing: ${header}")
                endif()
            endif()
        endforeach()
    else()
        message(FATAL_ERROR "OpenCV core target not found")
    endif()
else()
    find_package(OpenCV REQUIRED)
endif()


#-------------------------------------------------------------------------------
# SDL2
#-------------------------------------------------------------------------------
if(NOT ROBOTACT_USE_SYSTEM_DEPS OR ROBOTACT_FORCE_FETCH_DEPS)
    FetchContent_Declare(
        sdl2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.32.8  # Use a stable release
        GIT_SHALLOW TRUE
    )
    # Force static build unless shared is explicitly wanted
    set(SDL_SHARED ${BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)  # Prefer static linking
    FetchContent_MakeAvailable(sdl2)
else()
    find_package(SDL2 REQUIRED)
endif()

#-------------------------------------------------------------------------------
# GLM (Mathematics Library)
#-------------------------------------------------------------------------------
set(GLM_FORCE_CMAKE_DEPRECATION_WARNINGS OFF CACHE BOOL "" FORCE)

if(NOT TARGET glm)
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 1.0.1
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(glm)

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
        GIT_TAG docking
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
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PRIVATE SDL2::SDL2 OpenGL::GL)


#-------------------------------------------------------------------------------
# nlohmann/json (JSON Library)
#-------------------------------------------------------------------------------
if(NOT ROBOTACT_USE_SYSTEM_DEPS OR ROBOTACT_FORCE_FETCH_DEPS)
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.12.0
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
else()
    find_package(nlohmann_json 3.12.0 REQUIRED)
endif()

#-------------------------------------------------------------------------------
# TinyXML-2 (XML Library)
#-------------------------------------------------------------------------------
if(NOT ROBOTACT_USE_SYSTEM_DEPS OR ROBOTACT_FORCE_FETCH_DEPS)
    FetchContent_Declare(
            tinyxml2
            GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
            GIT_TAG 9.0.0
            GIT_SHALLOW TRUE
    )
    # TinyXML-2 build options
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(tinyxml2)
else()
    find_package(tinyxml2 9.0.0 QUIET)
    if(NOT tinyxml2_FOUND)
        message(STATUS "TinyXML-2 not found, falling back to FetchContent")
        FetchContent_Declare(
                tinyxml2
                GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
                GIT_TAG 9.0.0
                GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(tinyxml2)
    endif()
endif()

#-------------------------------------------------------------------------------
# GLAD (OpenGL Loader)
#-------------------------------------------------------------------------------
add_library(glad STATIC ${PROJECT_SOURCE_DIR}/dependencies/glad/src/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/glad/include)
target_link_libraries(glad PUBLIC OpenGL::GL)

if(UNIX AND NOT APPLE)
    target_compile_options(glad PRIVATE -w)
endif()

#-------------------------------------------------------------------------------
# STB (Single-file Libraries)
#-------------------------------------------------------------------------------
add_library(stb_image STATIC ${PROJECT_SOURCE_DIR}/dependencies/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/stb_image)

#-------------------------------------------------------------------------------
# Main Application
#-------------------------------------------------------------------------------
file(GLOB_RECURSE PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.h
)

add_executable(RoboTact ${PROJECT_SOURCES})
target_include_directories(RoboTact PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}
)

# Platform-specific linking
if(APPLE)
    message(STATUS "Configuring macOS-specific frameworks and sources")

    # macOS Frameworks required for AVFoundation and camera access
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(AVFOUNDATION_LIBRARY AVFoundation)
    find_library(COREMEDIA_LIBRARY CoreMedia)

#     # Add .mm source file for Objective-C++ camera backend
#     target_sources(RoboTact PRIVATE
#             ${PROJECT_SOURCE_DIR}/src/camera/camera_devices_mac.mm
#     )

    # Link all required macOS frameworks
    target_link_libraries(RoboTact PRIVATE
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
            ${AVFOUNDATION_LIBRARY}
            ${COREMEDIA_LIBRARY}
    )
endif()


target_link_libraries(RoboTact PRIVATE
        OpenCV::OpenCV
        SDL2::SDL2
        glad
        imgui
        stb_image
        glm
        OpenGL::GL
        nlohmann_json::nlohmann_json  # JSON support
        tinyxml2                      # XML support
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