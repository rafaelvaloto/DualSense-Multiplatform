// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/Structs/Context/InputContext.h"

#define DS_TOUCHPAD_WIDTH 1920
#define DS_TOUCHPAD_HEIGHT 1080
#define DS4_TOUCHPAD_WIDTH 1920
#define DS4_TOUCHPAD_HEIGHT 942

namespace FGamepadTouch
{
	inline void ProcessTouchDualSense(const unsigned char* HIDInput, FInputContext* Input)
	{
		if (Input->TouchRadius.X == 0.0f || Input->TouchRadius.Y == 0.0f)
		{
			Input->TouchRadius = {DS_TOUCHPAD_WIDTH, DS_TOUCHPAD_HEIGHT};
		}

		Input->TouchId = 1;
		Input->bIsTouching = (HIDInput[0x20] & 0x80) == 0;
		Input->DirectionRaw = HIDInput[0x28];

		const float AbsX = static_cast<float>(((HIDInput[0x22] & 0x0F) << 8) | HIDInput[0x21]) * 1.f;
		const float AbsY = static_cast<float>((HIDInput[0x23] << 4) | ((HIDInput[0x22] & 0xF0) >> 4)) * 1.f;
		Input->TouchPosition = {AbsX, AbsY};

		const float AbsRelativeX = static_cast<float>(((HIDInput[0x27] & 0x0F) << 8) | HIDInput[0x25]) * 1.f;
		const float AbsRelativeY = static_cast<float>((HIDInput[0x26] << 4) | ((HIDInput[0x27] & 0xF0) >> 4)) * 1.f;
		Input->TouchRelative = {AbsRelativeX, AbsRelativeY};

		Input->TouchFingerCount = Input->bIsTouching && (HIDInput[0x24] & 0x80) == 0 ? 2 : 1;
	}

	inline void ProcessTouchDualShock(const unsigned char* HIDInput, FInputContext* Input, EDSDeviceConnection ConnectionType)
	{
		if (Input->TouchRadius.X == 0.0f || Input->TouchRadius.Y == 0.0f)
		{
			Input->TouchRadius = {DS4_TOUCHPAD_WIDTH, DS4_TOUCHPAD_HEIGHT};
		}

		// On the DS4, touch data starts after the sensor and status fields.
		// USB: common (32 bytes) + num_touch_reports (1 byte) = 33 bytes.
		// BT: reserved (2 bytes) + common (32 bytes) + num_touch_reports (1 byte) = 35 bytes.
		const int TouchReportOffset = (ConnectionType == EDSDeviceConnection::Bluetooth) ? 35 : 33;

		const int Point0Offset = TouchReportOffset + 1;
		const unsigned char* p0 = &HIDInput[Point0Offset];

		Input->TouchId = p0[0] & 0x7F;
		Input->bIsTouching = (p0[0] & 0x80) == 0;

		if (Input->bIsTouching)
		{
			const auto AbsX = static_cast<float>(((p0[2] & 0x0F) << 8) | p0[1]);
			const auto AbsY = static_cast<float>((p0[3] << 4) | ((p0[2] & 0xF0) >> 4));
			Input->TouchPosition = {AbsX, AbsY};
			Input->TouchRelative = {0.0f, 0.0f};
		}

		const int Point1Offset = Point0Offset + 4;
		const unsigned char* p1 = &HIDInput[Point1Offset];
		const bool bSecondFinger = (p1[0] & 0x80) == 0;
		Input->TouchFingerCount = Input->bIsTouching ? (bSecondFinger ? 2 : 1) : 0;
	}

} // namespace FGamepadTouch
