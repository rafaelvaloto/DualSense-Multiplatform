// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/Structs/Context/InputContext.h"

#define DS_TOUCHPAD_WIDTH 1920
#define DS_TOUCHPAD_HEIGHT 1080

namespace FGamepadTouch
{
	inline bool IsTouching(const DSCoreTypes::DSVector2D& TouchPosition,
	                       const DSCoreTypes::DSVector2D& TouchRadius)
	{
		return TouchPosition.X >= 0 && TouchPosition.X <= TouchRadius.X &&
		       TouchPosition.Y >= 0 && TouchPosition.Y <= TouchRadius.Y;
	}

	inline void ProcessTouch(const unsigned char* HIDInput, FInputContext* Input)
	{
		if (Input->TouchRadius.X == 0.0f || Input->TouchRadius.Y == 0.0f)
		{
			Input->TouchRadius = {DS_TOUCHPAD_WIDTH, DS_TOUCHPAD_HEIGHT};
		}

		Input->TouchId = (HIDInput[0x20] & 0x7F) % 10;
		Input->bIsTouching = (HIDInput[0x20] & 0x80) != 0;
		Input->DirectionRaw = HIDInput[0x28];

		const float AbsX = static_cast<float>(((HIDInput[0x22] & 0x0F) << 8) | HIDInput[0x21]) * 1.f;
		const float AbsY = static_cast<float>((HIDInput[0x23] << 4) | ((HIDInput[0x22] & 0xF0) >> 4)) * 1.f;
		Input->TouchPosition = {AbsX, AbsY};

		const float AbsRelativeX = static_cast<float>(((HIDInput[0x27] & 0x0F) << 8) | HIDInput[0x25]) * 1.f;
		const float AbsRelativeY = static_cast<float>((HIDInput[0x26] << 4) | ((HIDInput[0x27] & 0xF0) >> 4)) * 1.f;
		Input->TouchRelative = {AbsRelativeX, AbsRelativeY};

		Input->P1_Last = Input->P1_Current;
		Input->P1_Current = Input->TouchPosition;

		Input->P2_Last = Input->P2_Current;
		Input->P2_Current = Input->TouchRelative;

		Input->TouchFingerCount =
		    Input->bIsTouching && (HIDInput[0x24] & 0x80) != 0 ? 2 : 1;
	}

} // namespace FGamepadTouch
