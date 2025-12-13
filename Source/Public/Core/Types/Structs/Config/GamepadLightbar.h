// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"

/**
 * @class FGamepadLightbar
 * @brief Represents a light bar with various properties and methods to control
 * its behavior.
 *
 * The FGamepadLightbar class provides an interface for managing and interacting
 * with a programmable light bar. It includes functionality to set and get light
 * patterns, manage brightness, and control other visual effects. This class is
 * typically used in applications where dynamic light effects are required.
 *
 * This class assumes that the underlying hardware is capable of supporting the
 * requested behaviors.
 *
 * Key features include:
 * - Control of brightness settings.
 * - Configuration and retrieval of light patterns.
 * - Management of power state for the light bar.
 */
struct FGamepadLightbar
{
	/**
	 * Represents the variable R, typically used to store a specific value or
	 * parameter relevant to the context of the application or computation.
	 *
	 * The purpose and usage of R depend on the implementation details within the
	 * given context. It could be used to hold a numeric value, an object, or any
	 * other data type as required by the program's logic.
	 *
	 * Ensure that the value assigned to R aligns with the intended use case and
	 * data type requirements of the application.
	 *
	 * Proper initialization and handling of R are crucial for maintaining program
	 * stability and preventing potential runtime errors.
	 */
	std::uint8_t R = 0x0;
	/**
	 * @brief Reverses a given string in place.
	 *
	 * This function modifies the input string to reverse its characters. It swaps
	 * characters from the beginning and end of the string, moving towards the
	 * center.
	 *
	 * @param str The string to be reversed. Passed by reference.
	 */
	std::uint8_t G = 0x0;
	/**
	 * @brief Represents a variable used for storing data or intermediate values
	 * within a program.
	 *
	 * The variable B may serve various purposes depending on its context within
	 * the application. It could hold numeric, textual, or other data types,
	 * depending on the implementation.
	 *
	 * @note Ensure the type and purpose of B are clearly defined in the
	 * surrounding codebase to avoid misuse.
	 */
	std::uint8_t B = 0x0;
	/**
	 * @brief Represents a variable used within the program.
	 *
	 * The purpose and functionality of the variable `A` depend on its
	 * context and usage within the codebase. It can be assigned a value
	 * or modified during execution based on program requirements.
	 */
	std::uint8_t A = 0x0;
};
