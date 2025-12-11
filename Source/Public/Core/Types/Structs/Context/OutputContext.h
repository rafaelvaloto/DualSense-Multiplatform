// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#pragma once
#include "Core/Types/DSCoreTypes.h"
#include "Core/Types/Structs/Config/GamepadAudio.h"
#include "Core/Types/Structs/Config/GamepadFeature.h"
#include "Core/Types/Structs/Config/GamepadLightbar.h"
#include "Core/Types/Structs/Config/GamepadLigthbarFlash.h"
#include "Core/Types/Structs/Config/GamepadPlayerLed.h"
#include "Core/Types/Structs/Config/GamepadRumbles.h"
#include "Core/Types/Structs/Config/GamepadTriggersHaptic.h"

using namespace DSCoreTypes;

/**
 * @class FOutputContext
 * @brief The FOutput class is responsible for managing and handling output
 * functionalities.
 *
 * This class provides methods and attributes that enable output operations,
 * such as processing, formatting, or directing the output to different
 * destinations. It may include support for various data types, customization of
 * output format, and other related functionalities required for output
 * handling.
 *
 * Responsibilities of the FOutput class:
 * - Enable efficient output of data to designated targets.
 * - Support customizable output formatting and styles.
 * - Provide mechanisms for error handling or logging during output operations.
 *
 * Design considerations:
 * - Ensure flexibility and extensibility to adapt to various output
 * destinations.
 * - Maintain compatibility with different output data formats if applicable.
 */
struct FOutputContext
{
	/**
	 * @brief Represents the Lightbar configuration for output operations.
	 *
	 * The Lightbar is used to control the RGB color output of the device. It is
	 * commonly used to visually convey device states or enhance user experience
	 * by changing colors. The R, G, B, and A components of the light color can be
	 * adjusted for customization.
	 */
	FGamepadLightbar Lightbar;
	/**
	 * @class FGamepadLigthbarFlash
	 *
	 * @brief Manages the lightbar functionality for a DualShock controller.
	 *
	 * The FGamepadLigthbarFlash class is designed to handle the control and
	 * customization of the lightbar on a DualShock controller. It provides
	 * functionality to define and implement light patterns, colors, and flashing
	 * behaviors suitable for various gaming or application scenarios.
	 *
	 * This class is intended to interact with the DualShock controller hardware,
	 * allowing dynamic updates to the lightbar as required by the application
	 * logic.
	 *
	 * @note Proper integration and hardware compatibility are required to ensure
	 *       the expected behavior of the lightbar.
	 */
	FGamepadLigthbarFlash FlashLigthbar;
	/**
	 * Represents the configuration of the player's LED on a device.
	 *
	 * FGamepadPlayerLed is used to control and specify the state of the player's
	 * indicator LED on a controller, including its brightness and which LED is
	 * active. It is utilized within the FOutput structure and controller output
	 * protocols to define LED behavior based on player or system states.
	 */
	FGamepadPlayerLed PlayerLed;
	/**
	 * Represents the rumble settings for a controller.
	 * This struct contains two unsigned char variables, Left and Right,
	 * which determine the intensities for the left and right rumble motors
	 * respectively.
	 *
	 * Used within FOutput to define the vibration feedback for the controller.
	 * The values for Left and Right range from 0x00 (no vibration) to 0xFF
	 * (maximum vibration).
	 *
	 * The Rumbles variable is utilized in multiple functions across the DualSense
	 * API, managing rumble intensities during feedback events.
	 */
	FGamepadRumbles Rumbles;
	/**
	 * Represents the audio configuration for a device. This variable encapsulates
	 * settings related to audio levels and microphone status.
	 *
	 * The Audio variable is part of a hierarchical output structure, which
	 * includes other device
	 *
	 * This variable interacts directly with the device's output buffer during
	 * data transfer, ensuring that audio-related parameters such as volume and
	 * microphone state are configured correctly.
	 *
	 * Fields:
	 * - Mode: Defines the audio mode or configuration setting.
	 * - HeadsetVolume: Specifies the volume level for the headset.
	 * - SpeakerVolume: Specifies the volume level for the speaker.
	 * - MicVolume: Defines the volume level for the microphone.
	 * - MicStatus: Indicates the status of the microphone, such as enabled or
	 * disabled.
	 */
	FGamepadAudio Audio;
	/**
	 * Represents configuration details for device features such as vibration,
	 * rumble, and trigger settings.
	 *
	 * The `Feature` variable is an instance of the FGamepadFeature structure,
	 * which contains the following:
	 *
	 * - `FeatureMode`: Defines the operational mode for features.
	 * - `VibrationMode`: Specifies the vibration mode for the device.
	 * - `SoftRumbleReduce`: Controls the reduction level for soft rumble effects.
	 * - `TriggerSoftnessLevel`: Determines the softness level for triggers.
	 *
	 * Used as part of the output configuration sent to the device, enabling
	 * customization and control over various aspects of the device's behavior.
	 */
	FGamepadFeature Feature;
	/**
	 * Represents the haptic feedback configuration for the left adaptive trigger
	 * of a DualSense controller. This variable is a part of the FOutput
	 * structure. It provides control over various trigger haptic effects such as
	 * vibration mode, amplitude, frequency, and active effect zones.
	 *
	 * The LeftTrigger variable interacts with various controller feedback systems
	 * for implementing precise and immersive trigger effects during gameplay.
	 *
	 * Key properties include:
	 * - Mode: Defines the type of haptic effect applied to the trigger.
	 * - Frequency: Sets the frequency of the vibration for fine-grained feedback.
	 * - Amplitude: Determines the strength of the haptic effect.
	 * - Strengths: Encapsulates zones and their corresponding strength levels.
	 *
	 */
	FGamepadTriggersHaptic LeftTrigger;
	/**
	 * Represents the haptic feedback configuration for the right trigger of a
	 * DualSense controller.
	 *
	 * The `RightTrigger` variable is used to define various haptic trigger
	 * settings. It includes functionality to control the mode, frequency,
	 * amplitude, and effects of the right trigger, enhancing tactile feedback for
	 * the user.
	 *
	 * This variable is a part of the controller output structure and interacts
	 * with the hardware through buffered outputs.
	 *
	 * Associated with trigger feedback adjustments:
	 * - Mode: Defines the operational mode of the right trigger.
	 * - Strengths: Configures active and strength zones for fine-tuned trigger
	 * behavior.
	 *
	 * Used primarily in functions such As
	 * - SetHapticFeedbackValues(): To modify frequency based on specific feedback
	 * values.
	 * - SetTriggers(): To adjust the mode and strength zones for the right
	 * trigger.
	 *
	 * Additionally, this variable is integrated into the output process, where
	 * its values are buffered and transmitted to the device.
	 *
	 * This configuration allows seamless customization for immersive user
	 * interaction.
	 */
	FGamepadTriggersHaptic RightTrigger;
};
