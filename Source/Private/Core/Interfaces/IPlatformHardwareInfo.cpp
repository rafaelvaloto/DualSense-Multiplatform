// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "Core/Interfaces/IPlatformHardwareInfo.h"

#if defined(_WIN32)
#include "Implementations/Platforms/Windows/WindowsDeviceInfo.h"
#elif defined(__unix__)
#include "Implementations/Platforms/Commons/CommonsDeviceInfo.h"
#elif defined(__APPLE__)
#include "Implementations/Platforms/Mac/FNullHardwareInterface.h" // Ou seu header real
#endif

std::unique_ptr<IPlatformHardwareInfo>
    IPlatformHardwareInfo::PlatformInfoInstance = nullptr;

IPlatformHardwareInfo& IPlatformHardwareInfo::Get()
{
	if (PlatformInfoInstance)
	{
		return *PlatformInfoInstance;
	}

#ifdef _WIN32
	PlatformInfoInstance = std::make_unique<FWindowsDeviceInfo>();
#elif defined(__unix__)
	PlatformInfoInstance = std::make_unique<FCommonsDeviceInfo>();
#else
	// PlatformInfoInstance = std::make_unique<FMacDeviceInfo>();
#endif

	// Se mesmo assim falhou (plataforma desconhecida e sem injeção)
	if (!PlatformInfoInstance)
	{
		// Aqui você pode decidir: lançar exceção ou retornar uma instância
		// NullObject que não faz nada throw std::runtime_error("Platform Hardware
		// Info not initialized!");
	}

	return *PlatformInfoInstance;
}

void IPlatformHardwareInfo::SetInstance(
    std::unique_ptr<IPlatformHardwareInfo> Instance)
{
	PlatformInfoInstance = std::move(Instance);
}
