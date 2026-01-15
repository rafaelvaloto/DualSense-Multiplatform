#pragma once
#if defined(_WIN32) && defined(USE_VIGEM)
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace GamepadCore {

class Logger {
public:
    static void Initialize(const std::string& directoryPath) {
        try {
            if (!std::filesystem::exists(directoryPath)) {
                std::filesystem::create_directories(directoryPath);
            }

            std::string logFilePath = directoryPath + "\\GamepadService.log";
            
            // Se o arquivo já existir e for muito grande, podemos rotacionar ou apenas abrir em modo append
            logFile.open(logFilePath, std::ios::out | std::ios::app);
            
            if (logFile.is_open()) {
                originalCoutBuffer = std::cout.rdbuf();
                std::cout.rdbuf(logFile.rdbuf());
                
                originalCerrBuffer = std::cerr.rdbuf();
                std::cerr.rdbuf(logFile.rdbuf());

                LogTimestamp();
                std::cout << "[Logger] System Initialized. Logging to: " << logFilePath << std::endl;
            }
        } catch (const std::exception& e) {
            // Se falhar ao inicializar o log no arquivo, continuamos com o console padrão
            // (ou podemos tratar de outra forma)
        }
    }

    static void Shutdown() {
        if (logFile.is_open()) {
            std::cout << "[Logger] System Shutting down." << std::endl;
            
            // Restaura os buffers originais
            if (originalCoutBuffer) {
                std::cout.rdbuf(originalCoutBuffer);
            }
            if (originalCerrBuffer) {
                std::cerr.rdbuf(originalCerrBuffer);
            }
            
            logFile.close();
        }
    }

private:
    static void LogTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::cout << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "] ";
    }

    static inline std::ofstream logFile;
    static inline std::streambuf* originalCoutBuffer = nullptr;
    static inline std::streambuf* originalCerrBuffer = nullptr;
};

} // namespace GamepadCore

#endif
