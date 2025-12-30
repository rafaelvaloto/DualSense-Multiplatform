// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#pragma once
#include "../../DSCoreTypes.h"
/**
 * @class FAudioContext
 *
 * @brief Represents the configuration settings for the audio system.
 *
 * The FAudioConfig class encapsulates various configuration parameters and
 * settings related to an audio system. It provides an interface to manage
 * audio configurations such as sample rate, channel count, and other audio
 * processing properties.
 *
 * This class is intended to be used as part of the audio system to define
 * or modify how audio is processed and output. It acts as a central place
 * for configuring audio-specific parameters.
 *
 * @note Ensure proper initialization of the configuration before using it
 *       with the audio processing pipeline.
 */
struct FAudioContext
{
	/**
	 * @brief Represents a specific operational state or behavior mode of a system
	 * or application.
	 *
	 * Mode is typically used to define the way a system, process, or
	 * functionality operates under different conditions or configurations. The
	 * values it can hold are often predefined and may correspond to distinct
	 * states like manual, automatic, debug, or maintenance modes.
	 *
	 * The exact interpretation of the Mode variable depends on the context in
	 * which it is used.
	 *
	 * Usage of this variable may influence the overall behavior, decision-making,
	 * or output of the system it is a part of.
	 */
	std::uint8_t Mode = 0x05;
	/**
	 * Represents the audio volume level for the connected headset.
	 *
	 * The `HeadsetVolume` variable controls the output volume for audio
	 * transmitted to the headset connected to a DualSense device. It is an 8-bit
	 * unsigned integer, with a default value of `0x7C` (124 in decimal). The
	 * value can be adjusted programmatically to set the desired volume level.
	 *
	 * This variable is primarily used in device output configurations for
	 * transmitting audio-related settings to a DualSense controller.
	 */
	std::uint8_t HeadsetVolume = 0x7C;
	/**
	 * Represents the volume level of the speaker output for a DualSense device.
	 *
	 * The `SpeakerVolume` variable is utilized to manage and control the audio
	 * output level of the device's speaker. Its value can range from 0 to 255
	 * (0x00 to 0xFF), where higher values indicate higher volume levels.
	 *
	 * Default Value: 0x7C (124 in decimal), which indicates an initial medium
	 * volume level.
	 *
	 * It is part of the `FAudioConfig` struct, which groups all the audio-related
	 * configurations for the Devices and is applied along with other features
	 * during HID report communication
	 *
	 * Usage:
	 * Used during DualSense Devices configuration and in HID report processing to
	 * adjust the speaker's output level.
	 */
	std::uint8_t SpeakerVolume = 0x7C;
	/**
	 * MicVolume is an 8-bit unsigned integer that represents the microphone
	 * volume level in an audio configuration. It is primarily used within the
	 * DualSense controller audio settings system to configure and control the
	 * microphone's output level.
	 *
	 * Default Value: 0x7C (124 in decimal), which is assumed to be a
	 * mid-to-high-level microphone volume setting.
	 *
	 * Usage Context:
	 * - The variable is part of the `FAudioConfig` struct, embedded within the
	 *   `FOutput` structure for managing controller output settings.
	 * - It is used during output buffer creation to send microphone volume data
	 *   to the hardware, particularly at buffer index `Output[6]` in a USB or
	 * Bluetooth context.
	 * - The value can be dynamically configured via the related API, such as the
	 *   `UDualSenseLibrary::Settings` method, using the provided settings
	 * structure.
	 */
	std::uint8_t MicVolume = 0x7C;
	/**
	 * @brief Represents the status of the microphone in DualSense audio settings.
	 *
	 * This variable is used to control and report the microphone status within
	 * the audio configuration settings of the DualSense device. Its value
	 * influences the output buffer and is transmitted to the device during
	 * communication.
	 *
	 * @details
	 * - Initial Value: 0x0
	 * - Encoding:
	 *   - 0x0: Microphone is off.
	 *   - 0x1: Microphone is on.
	 *
	 * Usage in Implementation:
	 * - This variable is part of the FAudioConfig structure and is modified
	 *   programmatically to reflect the desired microphone state.
	 * - Transmitted as part of the HID output data buffer during device
	 * communication.
	 * - Controlled via the settings API, influenced by user-defined
	 * configurations.
	 */
	std::uint8_t MicStatus = 0x0;
};
