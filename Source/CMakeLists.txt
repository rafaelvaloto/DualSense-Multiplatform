cmake_minimum_required(VERSION 3.20)
if(WIN32)
    add_compile_definitions(UNICODE _UNICODE)
endif()
# Collect sources/headers. CONFIGURE_DEPENDS makes CMake reconfigure when files are added/removed.
file(GLOB_RECURSE GAMEPADCORE_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp"
)

file(GLOB_RECURSE GAMEPADCORE_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cxx"
)

# Library target
add_library(GamepadCore STATIC
        ${GAMEPADCORE_HEADERS}
        ${GAMEPADCORE_SOURCES}
        ../Tests/Integration/main.test.cpp
        Public/GImplementations/Utils/GamepadAudio.h
)

target_include_directories(GamepadCore
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Public"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Private"
)

target_compile_features(GamepadCore PUBLIC cxx_std_20)

set_target_properties(GamepadCore PROPERTIES
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)

# ---- clang-format helpers (optional) ----
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if (CLANG_FORMAT_EXE)
    add_custom_target(GamepadCoreFormat
            COMMAND "${CLANG_FORMAT_EXE}" -i
            ${GAMEPADCORE_HEADERS}
            ${GAMEPADCORE_SOURCES}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Running clang-format on GamepadCore sources"
            VERBATIM
    )

    # If you REALLY want auto-format before each build, uncomment this:
    add_dependencies(GamepadCore GamepadCoreFormat)
endif()