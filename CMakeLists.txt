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

add_subdirectory(Source)

# Tests são opcionais - só inclui se o submodule foi inicializado
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Tests/CMakeLists.txt")
    # Global definition for test-specific code blocks
    add_definitions(-DBUILD_GAMEPAD_CORE_TESTS)

    option(BUILD_TESTS "Build integration tests" ON)
    option(AUTOMATED_TESTS "Enable automated tests (timeout for CI)" OFF)

    if(BUILD_TESTS)
        enable_testing()
        add_subdirectory(Tests)
    endif()
else()
    message(STATUS "Tests submodule not initialized - skipping tests")
endif()