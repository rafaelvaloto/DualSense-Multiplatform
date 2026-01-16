#if defined(_WIN32) && defined(USE_VIGEM)
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <windows.h>

namespace DSCoreTypes
{
	struct DSVector2D
	{
		float X, Y;
	};
	struct DSVector3D
	{
		float X, Y, Z;
	};
} // namespace DSCoreTypes

struct FInputContext
{
	float AnalogDeadZone;
	DSCoreTypes::DSVector2D LeftAnalog;
	DSCoreTypes::DSVector2D RightAnalog;
	float LeftTriggerAnalog;
	float RightTriggerAnalog;
	DSCoreTypes::DSVector3D Gyroscope;
	DSCoreTypes::DSVector3D Accelerometer;
	DSCoreTypes::DSVector3D Gravity;
	DSCoreTypes::DSVector3D Tilt;
	int32_t TouchId;
	int32_t TouchFingerCount;
	uint8_t DirectionRaw;
	bool bIsTouching;
	DSCoreTypes::DSVector2D TouchRadius;
	DSCoreTypes::DSVector2D TouchPosition;
	DSCoreTypes::DSVector2D TouchRelative;
	bool bCross, bSquare, bTriangle, bCircle;
	bool bDpadUp, bDpadDown, bDpadLeft, bDpadRight;
	bool bLeftAnalogRight, bLeftAnalogUp, bLeftAnalogDown, bLeftAnalogLeft;
	bool bRightAnalogLeft, bRightAnalogDown, bRightAnalogUp, bRightAnalogRight;
	bool bLeftTriggerThreshold, bRightTriggerThreshold;
	bool bLeftShoulder, bRightShoulder;
	bool bLeftStick, bRightStick;
	bool bPSButton, bShare, bStart, bTouch, bMute;
	bool bHasPhoneConnected;
	bool bFn1, bFn2, bPaddleLeft, bPaddleRight;
	float BatteryLevel;
};

typedef void (*StartServicePtr)();
typedef void (*StopServicePtr)();
typedef bool (*GetGamepadStateSafePtr)(int, FInputContext*);

int main()
{
	std::cout << "[Test] Iniciando Teste de Integracao da DLL do App..." << std::endl;

	const char* dllPath = "gaming-mods.dll";

	std::cout << "[Test] Carregando DLL de: " << dllPath << std::endl;
	HMODULE hDll = LoadLibraryA(dllPath);

	if (!hDll)
	{
		const char* dllName = "GamepadCoreApp.dll";
		std::cout << "[Test] Tentando carregar pelo nome: " << dllName << std::endl;
		hDll = LoadLibraryA(dllName);
	}

	if (!hDll)
	{
		DWORD error = GetLastError();
		std::cerr << "[Test] ERRO: Nao foi possivel carregar a DLL. Codigo: " << error << std::endl;

		char currentPath[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, currentPath);
		std::cerr << "[Test] Diretorio atual: " << currentPath << std::endl;

		std::cerr << "[Test] Certifique-se de que a GamepadCoreApp.dll esta no PATH ou na mesma pasta." << std::endl;
		return 1;
	}

	std::cout << "[Test] DLL carregada com sucesso." << std::endl;

	StartServicePtr StartService = (StartServicePtr)GetProcAddress(hDll, "StartGamepadService");
	StopServicePtr StopService = (StopServicePtr)GetProcAddress(hDll, "StopGamepadService");
	GetGamepadStateSafePtr GetGamepadStateSafe = (GetGamepadStateSafePtr)GetProcAddress(hDll, "GetGamepadStateSafe");

	if (!StartService || !StopService || !GetGamepadStateSafe)
	{
		std::cerr << "[Test] ERRO: Funcoes exportadas nao encontradas na DLL." << std::endl;
		FreeLibrary(hDll);
		return 1;
	}

	std::cout << "[Test] Chamando StartGamepadService()..." << std::endl;
	StartService();

#ifdef AUTOMATED_TESTS
	std::cout << "[Test] Modo automatizado ativo. Serviço rodando por 5 segundos para validação..." << std::endl;
	for (int i = 0; i < 50; ++i)
#else
	std::cout << "[Test] Modo local ativo. Pressione Ctrl+C para encerrar ou aguarde o fim do loop (50s)." << std::endl;
	for (int i = 0; i < 500; ++i)
#endif
	{
		FInputContext state;
		if (GetGamepadStateSafe(0, &state))
		{
			std::cout << "[Test] Estado do controle recebido! Bateria: " << state.BatteryLevel << "%" << std::endl;
			if (state.bCross)
			{
				std::cout << "      [!] Botão CROSS pressionado!" << std::endl;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "[Test] Chamando StopGamepadService()..." << std::endl;
	StopService();

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::cout << "[Test] Liberando DLL..." << std::endl;
	FreeLibrary(hDll);

	std::cout << "[Test] Teste concluido com sucesso." << std::endl;

	std::string logPath = "GamepadService.log";
	if (std::filesystem::exists(logPath))
	{
		std::cout << "[Test] Log verificado no diretorio local: " << logPath << std::endl;
		std::ifstream logFile(logPath);
		std::string line;
		std::cout << "--- CONTEUDO DO LOG ---" << std::endl;
		while (std::getline(logFile, line))
		{
			std::cout << line << std::endl;
		}
		std::cout << "-----------------------" << std::endl;
	}
	else
	{
		std::string oldLogPath = "C:\\Users\\rafae\\AppData\\Local\\SessionGame\\Saved\\Logs\\GamepadService.log";
		if (std::filesystem::exists(oldLogPath))
		{
			std::cout << "[Test] Log verificado no caminho absoluto: " << oldLogPath << std::endl;
		}
		else
		{
			std::cout << "[Test] ERRO: Log nao encontrado nem no local nem no caminho absoluto." << std::endl;
		}
	}

	return 0;
}
#endif
