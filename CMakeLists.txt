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


# ============================================
# Audio Module (opcional)
# ============================================
set(AUDIO_MODULE_PATH "${CMAKE_SOURCE_DIR}/Libs/miniaudio")

if(EXISTS "${AUDIO_MODULE_PATH}/miniaudio.h")
    target_sources(GamepadCore PRIVATE "${AUDIO_MODULE_PATH}/miniaudio_impl.cpp")
    target_include_directories(GamepadCore PRIVATE "${AUDIO_MODULE_PATH}")
    target_compile_definitions(GamepadCore PUBLIC GAMEPAD_CORE_HAS_AUDIO=1)
    message(STATUS "GamepadCore: Audio module ENABLED")
else()
    target_compile_definitions(GamepadCore PUBLIC GAMEPAD_CORE_HAS_AUDIO=0)
    message(STATUS "GamepadCore: Audio module not found")
    message(STATUS "  To enable: git submodule update --init Libs/miniaudio")
endif()

# ============================================
# Tests Module (opcional)
# ============================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Tests/Integration/CMakeLists.txt")
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