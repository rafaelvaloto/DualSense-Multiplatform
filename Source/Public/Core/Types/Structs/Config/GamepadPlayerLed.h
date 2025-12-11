// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"

struct FGamepadPlayerLed
{
	/**
	 * @brief Represents a LED (Light-Emitting Diode) control object.
	 *
	 * This variable is used to encapsulate the state and behavior of a LED. It
	 * can be used to manage the LED's on/off state, brightness, color, or other
	 * properties depending on the specific implementation.
	 *
	 * The exact behavior and attributes of the LED are dependent on the context
	 * in which the variable is implemented.
	 */
	std::uint8_t Led = 0x0;
	/**
	 * Represents the brightness level of the player's LED indicator on the
	 * controller.
	 *
	 * The value of this variable is an unsigned 8-bit integer (std::uint8_t) that
	 * can be adjusted to set the desired brightness level. The potential values
	 * range from 0x0 (minimum brightness or off) to the maximum value supported
	 * by the device.
	 *
	 * This variable is used in conjunction with player LED configurations
	 * and is passed to the output buffer to reflect the brightness level in
	 * real-time.
	 *
	 * Modifications to this variable impact the LED brightness visible on the
	 * device.
	 */
	std::uint8_t Brightness = 0x00;
};
