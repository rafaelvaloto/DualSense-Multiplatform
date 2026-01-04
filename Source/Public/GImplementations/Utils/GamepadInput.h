// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "GCore/Types/Structs/Context/DeviceContext.h"

namespace FGamepadInput
{
	inline void DualSenseRaw(const unsigned char* HIDInput, FInputContext* Input)
	{
		// Analogs
		const float LeftAnalogX = static_cast<float>(HIDInput[0x00] - 128) / 128.f;
		const float LeftAnalogY = static_cast<float>(HIDInput[0x01] - 128) / -128.f;
		const float RightAnalogX = static_cast<float>(HIDInput[0x02] - 128) / 128.f;
		const float RightAnalogY = static_cast<float>(HIDInput[0x03] - 128) / -128.f;

		const float TriggerL = HIDInput[0x04] / 256.0f;
		const float TriggerR = HIDInput[0x05] / 256.0f;

		uint8_t ButtonsMask = HIDInput[0x07] & 0xF0;
		const bool bCross = ButtonsMask & DSCoreTypes::InputMasks::Face::Cross;
		const bool bSquare = ButtonsMask & DSCoreTypes::InputMasks::Face::Square;
		const bool bCircle = ButtonsMask & DSCoreTypes::InputMasks::Face::Circle;
		const bool bTriangle = ButtonsMask & DSCoreTypes::InputMasks::Face::Triangle;

		switch (HIDInput[0x07] & 0x0F)
		{
			case 0x0:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Up;
				break;
			case 0x4:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Down;
				break;
			case 0x6:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Left;
				break;
			case 0x2:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Right;
				break;
			case 0x5:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Left |
				               DSCoreTypes::InputMasks::DPad::Down;
				break;
			case 0x7:
				ButtonsMask |=
				    DSCoreTypes::InputMasks::DPad::Left | DSCoreTypes::InputMasks::DPad::Up;
				break;
			case 0x1:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Right |
				               DSCoreTypes::InputMasks::DPad::Up;
				break;
			case 0x3:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Right |
				               DSCoreTypes::InputMasks::DPad::Down;
				break;
			default:;
		}

		const bool bDPadLeft = ButtonsMask & DSCoreTypes::InputMasks::DPad::Left;
		const bool bDPadDown = ButtonsMask & DSCoreTypes::InputMasks::DPad::Down;
		const bool bDPadRight = ButtonsMask & DSCoreTypes::InputMasks::DPad::Right;
		const bool bDPadUp = ButtonsMask & DSCoreTypes::InputMasks::DPad::Up;

		// Shoulders
		const bool bLeftShoulder =
		    HIDInput[0x08] & DSCoreTypes::InputMasks::Shoulder::L1;
		const bool bRightShoulder =
		    HIDInput[0x08] & DSCoreTypes::InputMasks::Shoulder::R1;

		// Push Stick
		const bool PushLeftStick =
		    HIDInput[0x08] & DSCoreTypes::InputMasks::Stick::L3;
		const bool PushRightStick =
		    HIDInput[0x08] & DSCoreTypes::InputMasks::Stick::R3;

		// Function & Special Actions
		const bool Playstation = HIDInput[0x09] & DSCoreTypes::InputMasks::Menu::PS;
		const bool TouchPad =
		    HIDInput[0x09] & DSCoreTypes::InputMasks::Menu::TouchPad;
		const bool Mic = HIDInput[0x09] & DSCoreTypes::InputMasks::Menu::Mute;
		const bool bFn1 = HIDInput[0x09] & DSCoreTypes::InputMasks::Extra::FnLeft;
		const bool bFn2 = HIDInput[0x09] & DSCoreTypes::InputMasks::Extra::FnRight;
		const bool bPaddleLeft =
		    HIDInput[0x09] & DSCoreTypes::InputMasks::Extra::PaddleLeft;
		const bool bPaddleRight =
		    HIDInput[0x09] & DSCoreTypes::InputMasks::Extra::PaddleRight;

		const bool Start = HIDInput[0x08] & DSCoreTypes::InputMasks::Menu::Options;
		const bool Select = HIDInput[0x08] & DSCoreTypes::InputMasks::Menu::Share;

		const bool bLeftTriggerThreshold =
		    HIDInput[0x08] & DSCoreTypes::InputMasks::Shoulder::L2;
		const bool bRightTriggerThreshold =
		    HIDInput[0x08] & DSCoreTypes::InputMasks::Shoulder::R2;

		Input->bHasPhoneConnected = (HIDInput[0x35] & 0x01);
		Input->BatteryLevel = static_cast<float>((static_cast<float>(HIDInput[0x34] & 0x0F) / 10.0) * 100);

		auto ApplyDeadZone = [](float Value, float Threshold) -> float {
			if (std::abs(Value) < Threshold)
			{
				return 0.0f;
			}

			const float Sign = (Value >= 0.0f ? 1.0f : -1.0f);
			const float Abs = std::abs(Value);
			return Sign * ((Abs - Threshold) / (1.0f - Threshold));
		};

		Input->LeftAnalog.X = ApplyDeadZone(LeftAnalogX, Input->AnalogDeadZone);
		Input->LeftAnalog.Y = ApplyDeadZone(LeftAnalogY, Input->AnalogDeadZone);
		Input->RightAnalog.X = ApplyDeadZone(RightAnalogX, Input->AnalogDeadZone);
		Input->RightAnalog.Y = ApplyDeadZone(RightAnalogY, Input->AnalogDeadZone);

		auto GetAnalogAsDigital = [](float Value, float Threshold) {
			return Value > Threshold;
		};

		const float DigitalThreshold = 0.5f;

		Input->bLeftAnalogRight = GetAnalogAsDigital(Input->LeftAnalog.X, DigitalThreshold);
		Input->bLeftAnalogLeft = GetAnalogAsDigital(-Input->LeftAnalog.X, DigitalThreshold);
		Input->bLeftAnalogDown = GetAnalogAsDigital(-Input->LeftAnalog.Y, DigitalThreshold);
		Input->bLeftAnalogUp = GetAnalogAsDigital(Input->LeftAnalog.Y, DigitalThreshold);

		Input->bRightAnalogRight = GetAnalogAsDigital(Input->RightAnalog.X, DigitalThreshold);
		Input->bRightAnalogLeft = GetAnalogAsDigital(-Input->RightAnalog.X, DigitalThreshold);
		Input->bRightAnalogDown = GetAnalogAsDigital(-Input->RightAnalog.Y, DigitalThreshold);
		Input->bRightAnalogUp = GetAnalogAsDigital(Input->RightAnalog.Y, DigitalThreshold);

		Input->LeftTriggerAnalog = TriggerL;
		Input->RightTriggerAnalog = TriggerR;
		Input->bCross = bCross;
		Input->bSquare = bSquare;
		Input->bCircle = bCircle;
		Input->bTriangle = bTriangle;
		Input->bDpadUp = bDPadUp;
		Input->bDpadDown = bDPadDown;
		Input->bDpadLeft = bDPadLeft;
		Input->bDpadRight = bDPadRight;
		Input->bLeftShoulder = bLeftShoulder;
		Input->bRightShoulder = bRightShoulder;
		Input->bStart = Start;
		Input->bShare = Select;
		Input->bLeftTriggerThreshold = bLeftTriggerThreshold;
		Input->bRightTriggerThreshold = bRightTriggerThreshold;
		Input->bLeftStick = PushLeftStick;
		Input->bRightStick = PushRightStick;

		Input->bMute = Mic;
		Input->bFn1 = bFn1;
		Input->bFn2 = bFn2;
		Input->bTouch = TouchPad;
		Input->bPSButton = Playstation;
		Input->bPaddleLeft = bPaddleLeft;
		Input->bPaddleRight = bPaddleRight;
	}

	inline void DualShockRaw(const unsigned char* HIDInput, FInputContext* Input)
	{
		// Triggers
		const bool bLeftTriggerThreshold =
		    HIDInput[0x05] & DSCoreTypes::InputMasks::Shoulder::L2;
		const bool bRightTriggerThreshold =
		    HIDInput[0x05] & DSCoreTypes::InputMasks::Shoulder::R2;

		// Triggers Analog 1D
		const float TriggerL = HIDInput[0x07] / 256.0f;
		const float TriggerR = HIDInput[0x08] / 256.0f;

		const float LeftAnalogX = static_cast<float>(HIDInput[0x00] - 128) / 128.f;
		const float LeftAnalogY = static_cast<float>(HIDInput[0x01] - 128) / -128.f;
		const float RightAnalogX = static_cast<float>(HIDInput[0x02] - 128) / 128.f;
		const float RightAnalogY = static_cast<float>(HIDInput[0x03] - 128) / -128.f;

		uint8_t ButtonsMask = HIDInput[0x04] & 0xF0;
		const bool bCross = ButtonsMask & DSCoreTypes::InputMasks::Face::Cross;
		const bool bSquare = ButtonsMask & DSCoreTypes::InputMasks::Face::Square;
		const bool bCircle = ButtonsMask & DSCoreTypes::InputMasks::Face::Circle;
		const bool bTriangle = ButtonsMask & DSCoreTypes::InputMasks::Face::Triangle;

		switch (HIDInput[0x04] & 0x0F)
		{
			case 0x0:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Up;
				break;
			case 0x4:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Down;
				break;
			case 0x6:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Left;
				break;
			case 0x2:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Right;
				break;
			case 0x5:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Left |
				               DSCoreTypes::InputMasks::DPad::Down;
				break;
			case 0x7:
				ButtonsMask |=
				    DSCoreTypes::InputMasks::DPad::Left | DSCoreTypes::InputMasks::DPad::Up;
				break;
			case 0x1:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Right |
				               DSCoreTypes::InputMasks::DPad::Up;
				break;
			case 0x3:
				ButtonsMask |= DSCoreTypes::InputMasks::DPad::Right |
				               DSCoreTypes::InputMasks::DPad::Down;
				break;
			default:;
		}

		const bool bDPadLeft = ButtonsMask & DSCoreTypes::InputMasks::DPad::Left;
		const bool bDPadDown = ButtonsMask & DSCoreTypes::InputMasks::DPad::Down;
		const bool bDPadRight = ButtonsMask & DSCoreTypes::InputMasks::DPad::Right;
		const bool bDPadUp = ButtonsMask & DSCoreTypes::InputMasks::DPad::Up;

		// Shoulders
		const bool bLeftShoulder =
		    HIDInput[0x05] & DSCoreTypes::InputMasks::Shoulder::L1;
		const bool bRightShoulder =
		    HIDInput[0x05] & DSCoreTypes::InputMasks::Shoulder::R1;

		// Push Stick
		const bool PushLeftStick =
		    HIDInput[0x05] & DSCoreTypes::InputMasks::Stick::L3;
		const bool PushRightStick =
		    HIDInput[0x05] & DSCoreTypes::InputMasks::Stick::R3;

		const bool Start = HIDInput[0x05] & DSCoreTypes::InputMasks::Menu::Options;
		const bool Select = HIDInput[0x05] & DSCoreTypes::InputMasks::Menu::Share;

		auto ApplyDeadZone = [](float Value, float Threshold) -> float {
			if (std::abs(Value) < Threshold)
			{
				return 0.0f;
			}

			const float Sign = (Value >= 0.0f ? 1.0f : -1.0f);
			const float Abs = std::abs(Value);
			return Sign * ((Abs - Threshold) / (1.0f - Threshold));
		};

		Input->LeftAnalog.X = ApplyDeadZone(LeftAnalogX, Input->AnalogDeadZone);
		Input->LeftAnalog.Y = ApplyDeadZone(LeftAnalogY, Input->AnalogDeadZone);
		Input->RightAnalog.X = ApplyDeadZone(RightAnalogX, Input->AnalogDeadZone);
		Input->RightAnalog.Y = ApplyDeadZone(RightAnalogY, Input->AnalogDeadZone);

		auto GetAnalogAsDigital = [](float Value, float Threshold) {
			return Value > Threshold;
		};

		const float DigitalThreshold = 0.5f;

		Input->bLeftAnalogRight = GetAnalogAsDigital(Input->LeftAnalog.X, DigitalThreshold);
		Input->bLeftAnalogLeft = GetAnalogAsDigital(-Input->LeftAnalog.X, DigitalThreshold);
		Input->bLeftAnalogDown = GetAnalogAsDigital(-Input->LeftAnalog.Y, DigitalThreshold);
		Input->bLeftAnalogUp = GetAnalogAsDigital(Input->LeftAnalog.Y, DigitalThreshold);

		Input->bRightAnalogRight = GetAnalogAsDigital(Input->RightAnalog.X, DigitalThreshold);
		Input->bRightAnalogLeft = GetAnalogAsDigital(-Input->RightAnalog.X, DigitalThreshold);
		Input->bRightAnalogDown = GetAnalogAsDigital(-Input->RightAnalog.Y, DigitalThreshold);
		Input->bRightAnalogUp = GetAnalogAsDigital(Input->RightAnalog.Y, DigitalThreshold);

		Input->LeftTriggerAnalog = TriggerL;
		Input->RightTriggerAnalog = TriggerR;
		Input->bCross = bCross;
		Input->bSquare = bSquare;
		Input->bCircle = bCircle;
		Input->bTriangle = bTriangle;
		Input->bDpadUp = bDPadUp;
		Input->bDpadDown = bDPadDown;
		Input->bDpadLeft = bDPadLeft;
		Input->bDpadRight = bDPadRight;
		Input->bLeftShoulder = bLeftShoulder;
		Input->bRightShoulder = bRightShoulder;
		Input->bLeftStick = PushLeftStick;
		Input->bRightStick = PushRightStick;
		Input->bStart = Start;
		Input->bShare = Select;
		Input->bLeftTriggerThreshold = bLeftTriggerThreshold;
		Input->bRightTriggerThreshold = bRightTriggerThreshold;
	}

} // namespace FGamepadInput
