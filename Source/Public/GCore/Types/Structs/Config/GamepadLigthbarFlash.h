// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "../../DSCoreTypes.h"

/**
 * Represents the player LED configuration for a device.
 */
struct FGamepadLigthbarFlash
{
	std::uint8_t Bright_Time = 0x0;
	std::uint8_t Toggle_Time = 0x0;
};
