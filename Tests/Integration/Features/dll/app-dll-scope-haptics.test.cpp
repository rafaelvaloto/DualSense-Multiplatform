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
	std::cout << "[Test] Iniciando Teste de Escopo de Haptics via DLL..." << std::endl;

	bool nonInteractive = false;

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

	std::cout << "[Test] O servico de haptics esta ativo via loopback de audio." << std::endl;
	std::cout << "[Test] Toque qualquer audio no sistema para sentir a vibracao no DualSense." << std::endl;

#ifdef AUTOMATED_TESTS
	std::cout << "[Test] Modo automatizado ativo. Encerrando em 5s..." << std::endl;
#else
	std::cout << "[Test] Pressione ENTER para encerrar o teste (ou aguarde 60s)..." << std::endl;

	std::atomic<bool> stopRequested(false);
	std::thread inputThread([&stopRequested]() {
		std::cin.get();
		stopRequested = true;
	});
#endif

	auto startTime = std::chrono::steady_clock::now();

	while (true)
	{
		FInputContext state;
		if (GetGamepadStateSafe(0, &state))
		{
		}

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();

#ifdef AUTOMATED_TESTS
		if (elapsed >= 5)
		{
			std::cout << "\n[Test] Tempo de teste atingido (5s). Encerrando..." << std::endl;
			break;
		}
#else
		if (stopRequested || elapsed >= 60)
		{
			if (stopRequested)
			{
				std::cout << "\n[Test] Encerrado pelo usuário." << std::endl;
			}
			else
			{
				std::cout << "\n[Test] Tempo limite atingido (60s)." << std::endl;
			}
			break;
		}
#endif

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

#ifndef AUTOMATED_TESTS
	if (inputThread.joinable())
	{
		std::cout << "[Test] Pressione ENTER para finalizar a thread de input..." << std::endl;
		inputThread.join();
	}
#endif

	std::cout << "\n[Test] Finalizando..." << std::endl;
	std::cout << "[Test] Chamando StopGamepadService()..." << std::endl;
	StopService();

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	FreeLibrary(hDll);

	std::cout << "[Test] Teste concluido." << std::endl;

	return 0;
}
#endif
