// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: GamepadCore - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once
#include <cstdint>

class IDeviceRegistry
{
public:
	virtual ~IDeviceRegistry() = default;
	virtual void PlugAndPlay(float DeltaTime) = 0;
};
