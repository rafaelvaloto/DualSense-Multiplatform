// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/Structs/Context/DeviceContext.h"
#include "Segregations/IGamepadAudioHaptics.h"
#include "Segregations/IGamepadLightbar.h"
#include "Segregations/IGamepadRumbles.h"
#include "Segregations/IGamepadSensors.h"
#include "Segregations/IGamepadStatus.h"
#include "Segregations/IGamepadTouch.h"
#include "Segregations/IGamepadTrigger.h"

/**
 * Interface defining the operations and configurations for a Sony gamepad
 * device. This interface must be implemented to interact with the gamepad,
 * handling tasks such as connection management, input updates, and device
 * settings.
 */
class ISonyGamepad : public IGamepadStatus,
                     public IGamepadRumbles,
                     public IGamepadLightbar,
                     public IGamepadSensors,
                     public IGamepadTouch
{

public:
	~ISonyGamepad() override = default;
	/**
	 * Initializes the Sony gamepad using the provided device context.
	 *
	 * This pure virtual method is responsible for setting up the gamepad
	 * instance by utilizing the given device context. The function must
	 * be implemented by any derived class to establish connection and
	 * prepare the gamepad for use.
	 *
	 * @param Context The device context containing information necessary
	 *                for initializing the gamepad.
	 * @return A boolean value indicating whether the initialization was
	 *         successful.
	 */
	virtual bool Initialize(const FDeviceContext& Context) = 0;
	/**
	 * Shuts down and cleans up resources related to the gamepad library.
	 *
	 * This method is responsible for properly deallocating and releasing any
	 * resources initialized or managed by the gamepad library. It should be
	 * called when the library is no longer needed to ensure no resource leaks
	 * or dangling references remain.
	 *
	 * Implementers must provide the logic to safely terminate any running
	 * processes, close device connections, and reset internal states as
	 * necessary.
	 *
	 * This function must be implemented by classes that inherit from the
	 * ISonyGamepad interface.
	 */
	virtual void ShutdownLibrary() = 0;
	/**
	 * Updates the output state of the gamepad.
	 *
	 * This pure virtual function is responsible for applying any changes
	 * or updates to the gamepad's output components, such as vibration,
	 * lightbar settings, or other feedback mechanisms. Derived classes
	 * must implement this method to ensure that the output states are
	 * correctly synchronized with the device hardware.
	 */
	virtual void UpdateOutput() = 0;
	/**
	 * Updates the input state for the Sony gamepad interface.
	 */
	virtual void UpdateInput(float Delta) = 0;
	/**
	 * Retrieves the gamepad trigger interface for managing trigger-specific
	 * features.
	 *
	 * This pure virtual function provides access to the IGamepadTrigger
	 * interface. It must be implemented by classes inheriting from the
	 * ISonyGamepad interface to enable handling and configuring the trigger
	 * functionality of the gamepad.
	 *
	 * @return A pointer to the IGamepadTrigger interface instance for the
	 * gamepad.
	 */
	virtual IGamepadTrigger* GetIGamepadTrigger() { return nullptr; }
	/**
	 * Provides access to the gamepad audio haptics interface for managing
	 * audio-driven haptic feedback effects.
	 *
	 * This pure virtual function allows for the retrieval of an instance of the
	 * IGamepadAudioHaptics interface, which can be used to configure audio-based
	 * haptic feedback on the gamepad.
	 *
	 * @return A pointer to the IGamepadAudioHaptics interface instance for the
	 * gamepad.
	 */
	virtual IGamepadAudioHaptics* GetIGamepadHaptics() { return nullptr; }
};
