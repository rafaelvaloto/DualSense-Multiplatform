// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"

using namespace DSCoreTypes;

class IGamepadTouch
{
public:
	virtual ~IGamepadTouch() = default;
	/**
	 * Sets the touch state for the device.
	 *
	 * @param bIsTouch A boolean indicating whether touch input is enabled (true)
	 * or disabled (false).
	 */
	virtual void EnableTouch(bool bIsTouch) = 0;
	/**
	 * Sets the touch state for the device.
	 *
	 * @param bIsGesture A boolean indicating whether touch input is enabled
	 * (true) or disabled (false).
	 */
	virtual void EnableGesture(bool bIsGesture) = 0;
};
