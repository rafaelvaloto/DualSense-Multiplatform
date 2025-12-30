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
    list(APPEND PLATFORM_LIBS Setupapi Hid)

elseif(UNIX AND NOT APPLE)
    # Configuração Linux (caso vá usar no futuro)
    list(APPEND PLATFORM_SOURCES
            "${CMAKE_SOURCE_DIR}/Examples/Platform_Linux/test_linux_device_info.cpp"
    )
    find_package(SDL2 REQUIRED)
    list(APPEND PLATFORM_LIBS ${SDL2_LIBRARIES})
    include_directories(${SDL2_INCLUDE_DIRS})
endif()


# 2. Criar o Executável
# Incluímos o main.test.cpp E o arquivo de implementação da plataforma (WindowsDeviceInfo.cpp)
add_executable(IntegrationTest
        main.test.cpp
        ${PLATFORM_SOURCES}
)

# 3. Configurar Includes
# O teste precisa achar os headers do Core e dos Examples (onde estão as Policies)
target_include_directories(IntegrationTest PRIVATE
        "${CMAKE_SOURCE_DIR}/Source/Public"
        "${CMAKE_SOURCE_DIR}/Source/Private"
        "${CMAKE_SOURCE_DIR}/Examples"
        "${CMAKE_SOURCE_DIR}"
)

# 4. Linkagem
# Linka com a sua lib estática (GamepadCore) e com as libs do Windows (Setupapi, Hid)
target_link_libraries(IntegrationTest
        PRIVATE
        GamepadCore
        ${PLATFORM_LIBS}
)