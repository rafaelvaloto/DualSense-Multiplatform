
cmake_minimum_required(VERSION 3.20)

# Auto-format sources with clang-format before building
file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
)

# Library for the WindowsDualsense (ds5w) core
add_library(GamepadCore STATIC
        ${ALL_CXX_SOURCE_FILES}
)

target_include_directories(GamepadCore
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

target_compile_features(GamepadCore PUBLIC cxx_std_20)

set_target_properties(GamepadCore PROPERTIES
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)
