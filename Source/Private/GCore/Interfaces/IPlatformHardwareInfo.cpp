// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "GCore/Interfaces/IPlatformHardwareInfo.h"

std::unique_ptr<IPlatformHardwareInfo>
    IPlatformHardwareInfo::PlatformInfoInstance = nullptr;

IPlatformHardwareInfo& IPlatformHardwareInfo::Get()
{
	if (PlatformInfoInstance)
	{
		return *PlatformInfoInstance;
	}

#ifdef _WIN32
	// PlatformInfoInstance = std::make_unique<FWindowsDeviceInfo>();
#elif defined(__unix__)
	// PlatformInfoInstance = std::make_shared<FCommonsDeviceInfo>();
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
    std::unique_ptr<IPlatformHardwareInfo> InPlatform)
{
	PlatformInfoInstance = std::move(InPlatform);
}
