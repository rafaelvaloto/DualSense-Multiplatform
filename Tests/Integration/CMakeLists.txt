cmake_minimum_required(VERSION 3.20)

project(GamepadCoreIntegrationTest)

# Define o padrão C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IMPORTANTE: Adiciona definições de Unicode para evitar erro de conversão de CHAR* para LPCWSTR
# e para garantir que linke com as versões corretas das APIs do Windows (CreateFileW, etc)
if(WIN32)
    add_compile_definitions(UNICODE _UNICODE)
endif()

# 1. Configurar Fontes e Libs da Plataforma
set(PLATFORM_SOURCES "")
set(PLATFORM_LIBS "")
if(WIN32)
    # No Windows, precisamos compilar a implementação do WindowsDeviceInfo
    # O CMAKE_SOURCE_DIR aponta para a raiz do projeto (GamepadCore)
    list(APPEND PLATFORM_SOURCES
            "${CMAKE_SOURCE_DIR}/Examples/Platform_Windows/test_windows_device_info.cpp"
    )

    # Bibliotecas do sistema obrigatórias para HID e SetupAPI
    # + bibliotecas necessárias para miniaudio (backend WASAPI usa COM)
    list(APPEND PLATFORM_LIBS
        Setupapi
        Hid
        ole32      # COM - usado pelo miniaudio/WASAPI
        oleaut32   # OLE Automation
        uuid       # GUIDs para interfaces COM
        winmm      # Multimedia APIs - necessário para miniaudio
    )

elseif(UNIX AND NOT APPLE)
    # Configuração Linux (caso vá usar no futuro)
    list(APPEND PLATFORM_SOURCES
            "${CMAKE_SOURCE_DIR}/Examples/Platform_Linux/test_linux_device_info.cpp"
    )
    find_package(SDL2 REQUIRED)
    list(APPEND PLATFORM_LIBS ${SDL2_LIBRARIES})
    include_directories(${SDL2_INCLUDE_DIRS})
endif()


# 2. Criar os Executáveis
# Incluímos o adaptive-triggers.test.cpp E o arquivo de implementação da plataforma (WindowsDeviceInfo.cpp)
add_executable(tests-adaptive-triggers
        Features/adaptive-triggers.test.cpp
        ${PLATFORM_SOURCES}
)

# Audio Haptics Integration Test - Lê arquivo .wav e envia para o controller
add_executable(tests-audio-haptics
        Features/audio-haptics.test.cpp
        ${PLATFORM_SOURCES}
)

# 3. Configurar Includes
# O teste precisa achar os headers do Core e dos Examples (onde estão as Policies)
set(COMMON_INCLUDES
        "${CMAKE_SOURCE_DIR}/Source/Public"
        "${CMAKE_SOURCE_DIR}/Source/Private"
        "${CMAKE_SOURCE_DIR}/Examples/Platform_Linux"
        "${CMAKE_SOURCE_DIR}/Examples/Platform_Windows"
        "${CMAKE_SOURCE_DIR}"
)

target_include_directories(tests-adaptive-triggers PRIVATE ${COMMON_INCLUDES})
target_include_directories(tests-audio-haptics PRIVATE ${COMMON_INCLUDES})

# Adiciona o caminho da raiz do projeto como uma definição de compilação
target_compile_definitions(tests-audio-haptics PRIVATE GAMEPAD_CORE_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

# 4. Linkagem
# Linka com a sua lib estática (GamepadCore) e com as libs do Windows (Setupapi, Hid)
target_link_libraries(tests-adaptive-triggers
        PRIVATE
        GamepadCore
        ${PLATFORM_LIBS}
)

target_link_libraries(tests-audio-haptics
        PRIVATE
        GamepadCore
        ${PLATFORM_LIBS}
)
