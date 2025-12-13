// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"
#include "GamepadTriggers.h"

/**
 * @class FGamepadTriggersHaptic
 * @brief Represents haptic feedback functionalities for triggers in a game
 * context.
 *
 * The FHapticTriggers class is designed to control and simulate haptic feedback
 * for trigger inputs in supported devices. This allows developers to integrate
 * tactile sensations for enhanced user immersion and interactivity.
 *
 * This class provides a structure to define properties and features for haptic
 * trigger effects, such as intensity, vibration pattern, and other
 * configuration options that are specific to the haptic feedback mechanism.
 *
 * It is typically used in conjunction with input devices, such as game
 * controllers or other peripherals, to create a responsive and engaging user
 * experience.
 */
struct FGamepadTriggersHaptic
{
	/**
	 * @brief This function initializes the configuration settings required for
	 * the application.
	 *
	 * This function is responsible for setting up all the necessary initial
	 * parameters and configurations for the system. It ensures that the required
	 * environment and settings are prepared before further processing.
	 *
	 * @param configFilePath A string representing the path to the configuration
	 * file.
	 * @param isDebugMode A boolean indicating whether the debug mode is enabled.
	 *
	 * @return Returns true if the initialization is successful, otherwise false.
	 */
	std::uint8_t Mode = 0x0;
	/**
	 * @brief Represents the strength configuration used in haptic feedback
	 * effects for triggers.
	 *
	 * This variable is an instance of the FStrengths struct, which defines
	 * various attributes relating to haptic feedback, including timing, active
	 * zones, time and ratio characteristics, and strength zones. It is utilized
	 * in programs to configure and apply detailed haptic response patterns.
	 */
	FGamepadTriggers Strengths;
};
