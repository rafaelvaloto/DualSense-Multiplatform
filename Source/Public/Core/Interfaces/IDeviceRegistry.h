// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: GamepadCore - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once
#include "ISonyGamepad.h"
#include <cstdint>

using CoreDeviceId = std::int32_t;

class IDeviceRegistry
{

public:
	virtual ~IDeviceRegistry() = default;
	virtual void PlugPlay(float DeltaTime) = 0;
	virtual void CreateLibraryInstance(FDeviceContext& Context) = 0;
	virtual void RemoveLibraryInstance(const CoreDeviceId& DeviceId) = 0;
	virtual ISonyGamepad* GetLibraryInstance(const CoreDeviceId& DeviceId) = 0;
};
