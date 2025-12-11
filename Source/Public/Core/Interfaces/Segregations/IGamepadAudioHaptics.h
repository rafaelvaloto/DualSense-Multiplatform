// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"

/**
 *
 */
class IGamepadAudioHaptics
{
	// Add interface functions to this class. This is the class that will be
	// inherited to implement this interface.
public:
	virtual ~IGamepadAudioHaptics() = default;
	/**
	 * Updates the haptic feedback on a gamepad's triggers using audio waveform
	 * data.
	 *
	 * @param AudioData An array of integer values representing the audio waveform
	 * data used to drive the haptic feedback effects on the triggers.
	 */
	virtual void AudioHapticUpdate(std::vector<std::uint8_t> AudioData) = 0;
};
