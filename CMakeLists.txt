cmake_minimum_required(VERSION 3.20)

# Main project name
project(GamepadCore LANGUAGES CXX)

# --- Global C++ Settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Unicode support for Windows targets
if(WIN32)
    add_compile_definitions(UNICODE _UNICODE)
endif()

# Global definition for test-specific code blocks
add_definitions(-DBUILD_GAMEPAD_CORE_TESTS)

# --- Include Project Subdirectories ---
# Process the library source code
add_subdirectory(Source)

# Option to enable or disable integration tests (default is ON)
option(BUILD_TESTS "Build integration tests" ON)

if(BUILD_TESTS)
    # Process the integration tests folder
    add_subdirectory(Tests/Integration)
endif()