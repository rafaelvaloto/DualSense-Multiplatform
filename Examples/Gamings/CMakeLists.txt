cmake_minimum_required(VERSION 3.20)

project(GamepadAppDLL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32 AND USE_VIGEM)
    set(MOD_SOURCES
            gaming-mods.cpp
            ../Platform_Windows/test_windows_device_info.cpp
            ../Platform_Windows/ViGEmAdapter/ViGEmAdapter.cpp
    )

    add_subdirectory(../Platform_Windows/ViGEmAdapter/ViGEmClient-master ViGEmClient)

    add_library(gaming-mods SHARED ${MOD_SOURCES})

    target_include_directories(gaming-mods PRIVATE
            ../Platform_Windows/ViGEmAdapter/ViGEmClient-master/include
    )

    target_compile_definitions(gaming-mods PRIVATE
        BUILD_GAMEPAD_CORE_TESTS
        USE_VIGEM
    )

    target_link_libraries(gaming-mods PRIVATE
            GamepadCore
            Setupapi
            Hid
            dxgi
            ole32
            oleaut32
            uuid
            winmm
            ViGEmClient::ViGEmClient
    )

    set_target_properties(gaming-mods PROPERTIES
            OUTPUT_NAME "gaming-mods"
            PREFIX ""
    )
else()
    message(STATUS "gaming-mods is disabled (requires Windows and USE_VIGEM=ON)")
endif()