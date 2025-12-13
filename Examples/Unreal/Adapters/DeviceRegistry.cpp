// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025

#include "Implementations/Adapters/DeviceRegistry.h"
#include "Core/Interfaces/ISonyGamepad.h"
#include "HAL/PlatformProcess.h"
#include "Runtime/Launch/Resources/Version.h"

TUniquePtr<FDeviceRegistry> FDeviceRegistry::Instance;
std::unique_ptr<FDeviceRegistry::FRegistryLogic> FDeviceRegistry::RegistryImplementation = nullptr;

FDeviceRegistry::~FDeviceRegistry()
{
}

FDeviceRegistry::FDeviceRegistry()
{
	RegistryImplementation = std::make_unique<FRegistryLogic>();
}

void FDeviceRegistry::Initialize()
{
	if (!Instance)
	{
		check(IsInGameThread());
		Instance.Reset(new FDeviceRegistry());
	}
}

void FDeviceRegistry::Shutdown()
{
	check(IsInGameThread());
	Instance.Reset();
}

FDeviceRegistry* FDeviceRegistry::Get()
{
	check(IsInGameThread());
	return Instance.Get();
}

void FDeviceRegistry::DiscoverDevices(float DeltaTime)
{
	if (RegistryImplementation)
	{
		return RegistryImplementation->PlugAndPlay(DeltaTime);
	}
}

ISonyGamepad* FDeviceRegistry::GetLibraryInstance(FInputDeviceId DeviceId)
{
	if (RegistryImplementation)
	{
		return RegistryImplementation->GetLibrary(DeviceId);
	}
	return nullptr;
}


