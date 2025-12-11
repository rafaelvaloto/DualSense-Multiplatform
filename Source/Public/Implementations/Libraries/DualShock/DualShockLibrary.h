// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualShock_ds5w - Plugin to support DualShock controller
// on Windows. Planned Release Year: 2025
#pragma once
#include "Core/Types/DSCoreTypes.h"
#include "Core/Types/Structs/Context/DeviceContext.h"
#include "Implementations/Libraries/Base/SonyGamepadAbstract.h"

class FDualShockLibrary : public SonyGamepadAbstract
{

public:
	/**
	 * @brief Initializes the DualShock library with the specified device context.
	 *
	 * This method sets up the library with the provided device context,
	 * configuring the necessary states for the DualShock controller, such as
	 * enabling or disabling certain features. It ensures the library is prepared
	 * to handle input from a connected DualShock device.
	 *
	 * @param Context A reference to an FDeviceContext structure that contains
	 * information about the current device, such as the connection type and
	 * configuration.
	 *
	 * @return Returns true if the library was successfully initialized.
	 */
	bool Initialize(const FDeviceContext& Context) override;
	/**
	 * @brief Sends output data to the connected DualShock controller.
	 *
	 * This method is responsible for transmitting data to the DualShock
	 * controller through the HID device context. It ensures that the
	 * output buffering mechanism processes the data correctly before
	 * managing communication with the controller.
	 *
	 * @details The method first verifies the connection status of the HID
	 * device context. If the device is not connected, the operation is aborted.
	 * Otherwise, it delegates the responsibility of handling the output
	 * buffering to the appropriate manager, ensuring proper data flow to the
	 * device.
	 */
	void UpdateOutput() override;

	/**
	 * @brief Updates the input state for a DualShock device.
	 *
	 * This method processes input data received from a DualShock controller and
	 * updates the application's state via the provided message handler. It
	 * ensures that the input is correctly mapped and associated with the
	 * specified platform user and input device.
	 * @return A boolean value indicating whether the input update was successful.
	 */
	void UpdateInput(float Delta) override;

	/**
	 * @brief Configures the lightbar settings of the DualShock controller.
	 *
	 * This method allows control over the lightbar's color and flashing behavior
	 * on the connected DualShock controller. The color of the lightbar can be set
	 * using the specified RGB values, while the brightness time and toggle time
	 * determine its flashing pattern.
	 *
	 * @param Color An FColor object representing the desired RGB color of the
	 * lightbar.
	 * @param BrithnessTime A float value representing the duration (in seconds)
	 * for which the lightbar stays bright during a flash cycle.
	 * @param ToggleTime A float value representing the duration (in seconds) for
	 * which the lightbar is toggled off during a flash cycle.
	 */
	void SetLightbar(FDSColor Color, float BrithnessTime, float ToggleTime) override;
	/**
	 * Stops all currently active operations or actions associated with the
	 * interface. This method must be implemented by any derived class to handle
	 * the termination of all ongoing processes.
	 */
	void ResetLights() override;
	/**
	 * @brief Sets the vibration strength for the DualShock controller.
	 *
	 * This function allows adjustment of the vibration motors in the controller,
	 * enabling haptic feedback customization. The intensity of the vibration can
	 * be independently set for the left and right motors.
	 *
	 * @param LeftRumble The intensity of the left motor's vibration (0-255).
	 * Optional, defaults to 0.
	 * @param RightRumble The intensity of the right motor's vibration (0-255).
	 * Optional, defaults to 0.
	 */
	void SetVibration(uint8_t LeftRumble, uint8_t RightRumble) override;
};
