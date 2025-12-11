cmake_minimum_required(VERSION 3.20)

# Root CMake for GamepadCore (engine-agnostic core)
project(GamepadCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Source)
