// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

class IGamepadRumbles
{
public:
	virtual ~IGamepadRumbles() = default;
	/**
	 * Sets the vibration levels for the gamepad.
	 *
	 * @param LeftRumble The intensity of the left-side vibration motor. The value
	 * should range from 0 (no vibration) to 255 (maximum intensity). Defaults to
	 * 0 if not specified.
	 * @param RightRumble The intensity of the right-side vibration motor. The
	 * value should range from 0 (no vibration) to 255 (maximum intensity).
	 * Defaults to 0 if not specified.
	 */
	virtual void SetVibration(std::uint8_t LeftRumble, std::uint8_t RightRumble) = 0;
};
