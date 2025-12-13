// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"

/**
 * Represents a collection of strength-related configurations and parameters for
 * haptic feedback. This struct is primarily used to define zones, timings, and
 * adjustable parameters for haptic effects on controller triggers.
 */
struct FGamepadTriggers
{
	std::uint8_t Compose[10] = {0};
};
