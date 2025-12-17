// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "../../DSCoreTypes.h"
#include "../../ECoreGamepad.h"

/**
 * @struct FInputContext
 * @brief Represents a structure that captures the current input state of
 * various analog inputs, motion sensors, and button states from a controller or
 * input device.
 *
 * This structure is used to encapsulate the state of inputs such as analog
 * sticks, triggers, motion sensors, and digital button presses for a given
 * frame or context.
 *
 * Members:
 *
 * ### Analog Inputs
 * - LeftAnalog: 2D vector representing the position of the left analog stick.
 * - RightAnalog: 2D vector representing the position of the right analog stick.
 * - LeftTriggerAnalog: Float value representing the analog input of the left
 * trigger.
 * - RightTriggerAnalog: Float value representing the analog input of the right
 * trigger.
 *
 * ### Motion Sensor Data
 * - Gyroscope: 3D vector representing the angular velocity measured by the
 * gyroscope.
 * - Accelerometer: 3D vector representing the linear acceleration detected by
 * the accelerometer.
 * - Gravity: 3D vector representing the gravity vector.
 * - Tilt: 3D vector representing the tilt or orientation of the device.
 *
 * ### Standard Buttons
 * - bCross: Indicates if the "Cross" button is pressed.
 * - bSquared: Indicates if the "Square" button is pressed.
 * - bTriangle: Indicates if the "Triangle" button is pressed.
 * - bCircle: Indicates if the "Circle" button is pressed.
 * - bDpadUp: Indicates if the D-pad "Up" button is pressed.
 * - bDpadDown: Indicates if the D-pad "Down" button is pressed.
 * - bDpadLeft: Indicates if the D-pad "Left" button is pressed.
 * - bDpadRight: Indicates if the D-pad "Right" button is pressed.
 *
 * ### Special Buttons
 * - bLeftTrigger: Indicates if the left trigger button is pressed.
 * - bRightTrigger: Indicates if the right trigger button is pressed.
 * - bLeftShoulder: Indicates if the left shoulder button is pressed.
 * - bRightShoulder: Indicates if the right shoulder button is pressed.
 * - bLeftStick: Indicates if the left stick button is pressed.
 * - bRightStick: Indicates if the right stick button is pressed.
 * - bPSButton: Indicates if the PlayStation/PS button is pressed.
 * - bShare: Indicates if the "Share" button is pressed.
 * - bStart: Indicates if the "Start" button is pressed.
 * - bTouch: Indicates if the touch input is active.
 * - bMute: Indicates if the "Mute" button is pressed.
 */
struct FInputContext
{
	// temporary
	float AnalogDeadZone = 0.08f;

	// Analogs
	DSCoreTypes::DSVector2D LeftAnalog = {0, 0};
	DSCoreTypes::DSVector2D RightAnalog = {0, 0};
	float LeftTriggerAnalog = 0.0f;
	float RightTriggerAnalog = 0.0f;

	DSCoreTypes::DSVector3D Gyroscope = {0, 0, 0};
	DSCoreTypes::DSVector3D Accelerometer = {0, 0, 0};
	DSCoreTypes::DSVector3D Gravity = {0, 0, 0};
	DSCoreTypes::DSVector3D Tilt = {0, 0, 0};

	// touch
	std::int32_t TouchId = 0;
	std::int32_t TouchFingerCount = 0;
	std::uint8_t DirectionRaw = 0;
	bool bIsTouching = false;

	DSCoreTypes::DSVector2D TouchRadius = {0, 0};
	DSCoreTypes::DSVector2D TouchPosition = {0, 0};
	DSCoreTypes::DSVector2D TouchRelative = {0, 0};

	// Buttons
	bool bCross;
	bool bSquare;
	bool bTriangle;
	bool bCircle;
	bool bDpadUp;
	bool bDpadDown;
	bool bDpadLeft;
	bool bDpadRight;

	// Special Buttons
	bool bLeftTriggerThreshold;
	bool bRightTriggerThreshold;
	bool bLeftShoulder;
	bool bRightShoulder;
	bool bLeftStick;
	bool bRightStick;
	bool bPSButton;
	bool bShare;
	bool bStart;
	bool bTouch;
	bool bMute;
	bool bHasPhoneConnected;

	// Edge
	bool bFn1;
	bool bFn2;
	bool bPaddleLeft;
	bool bPaddleRight;

	float BatteryLevel;
};
