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

# Option to enable or disable integration tests (default is ON)
option(BUILD_TESTS "Build integration tests" ON)
option(BUILD_DLL_TESTS "Build DLL integration tests" OFF)
option(USE_VIGEM "Enable ViGEm support" ON)
option(AUTOMATED_TESTS "Enable automated tests (timeout for CI)" OFF)

if(BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(Examples/Gamings)
add_subdirectory(Source)

if(BUILD_TESTS)
    # Process the integration tests folder
    add_subdirectory(Tests/Integration)
endif()