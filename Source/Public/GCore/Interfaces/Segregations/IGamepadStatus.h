// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#pragma once

#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/ECoreGamepad.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"

class IGamepadStatus
{
public:
	virtual ~IGamepadStatus() = default;
	/**
	 * Pure virtual function that checks the connection status of the gamepad.
	 *
	 * @return True if the gamepad is connected, false otherwise.
	 */
	virtual bool IsConnected() = 0;
	/**
	 * Retrieves the current battery level of the Sony gamepad.
	 *
	 * @return The battery level as a floating-point value, where the exact range
	 *         and representation are dependent on the implementation. Typically,
	 *         values may range between 0.0 (empty) and 1.0 (full).
	 */
	virtual float GetBattery() = 0;
	/**
	 * Retrieves the type of the device.
	 *
	 * @return The type of the device as an EDeviceType enumeration.
	 */
	virtual EDSDeviceType GetDeviceType() = 0;
	/**
	 * Retrieves the connection type of the device.
	 *
	 * @return The connection type of the device as an EDeviceConnection
	 * enumeration.
	 */
	virtual EDSDeviceConnection GetConnectionType() = 0;
	/**
	 * @brief Retrieves a mutable device context associated with the object.
	 *
	 * This method provides access to the device context in a mutable form,
	 * allowing modifications to the underlying hardware interface descriptor
	 * (HID) data specific to the device.
	 *
	 * @return A pointer to the mutable FDeviceContext object corresponding to the
	 * device's HID context.
	 */
	virtual FDeviceContext* GetMutableDeviceContext() = 0;
};
