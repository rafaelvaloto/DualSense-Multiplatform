// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

/**
 * @brief Enum class representing various LED microphone states.
 */
enum class EDSMic : std::uint8_t
{
	MicOn = 0,
	MicOff = 1,
	Pulse = 2
};

/**
 * @brief Player LED definition logic.
 */
enum class EDSPlayer : std::uint8_t
{
	Off = 0,
	One = DSCoreTypes::LedMasks::Middle,
	Two = DSCoreTypes::LedMasks::MiddleRight | DSCoreTypes::LedMasks::MiddleLeft,
	Three = DSCoreTypes::LedMasks::Right | DSCoreTypes::LedMasks::Middle | DSCoreTypes::LedMasks::Left,
	All = DSCoreTypes::LedMasks::Right | DSCoreTypes::LedMasks::MiddleRight | DSCoreTypes::LedMasks::MiddleLeft |
	      DSCoreTypes::LedMasks::Left
};

/**
 * @brief Audio feature states.
 */
enum class EDSAudioMode : std::uint8_t
{
	On,
	Off
};

enum class EDSVibrationMode : std::uint8_t
{
	Off = 0,
	DefaultRumble = 0xFF,
	HapticsRumble = 0xFC
};

enum class EDSGamepadHand : std::uint8_t
{
	Left,
	Right,
	AnyHand
};

enum class EDSTouchInteraction : std::uint8_t
{
	None,
	Swipe,
	Zoom,
	Scroll
};

enum class EDSDeviceType : std::uint8_t
{
	DualSense,
	DualSenseEdge,
	DualShock4,
	NotFound
};

enum class EDSDeviceConnection : std::uint8_t
{
	Usb,
	Bluetooth,
	Unrecognized
};

class ECoreGamepad
{
};
