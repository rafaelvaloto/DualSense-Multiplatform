// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#pragma once
#include "Core/Types/DSCoreTypes.h"
using namespace DSCoreTypes;

class IGamepadSensors
{
public:
	virtual ~IGamepadSensors() = default;
	/**
	 * Resets the orientation of the gyroscope to its default state.
	 * Typically used to recalibrate the gyroscope sensor.
	 */
	virtual void ResetGyroOrientation() = 0;
	/**
	 * Enables the motion sensor functionality of the gamepad.
	 *
	 * @param bIsMotionSensor Specifies whether to enable the gyroscope (true) or
	 * accelerometer (false) as the motion sensor.
	 */
	virtual void EnableMotionSensor(bool bIsMotionSensor) = 0;
};
