// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"

namespace FDualSenseTriggerComposer
{
	/**
	 * Disables the trigger functionality for the specified hand or hands on the
	 * provided device context.
	 *
	 * @param Context A pointer to the device context which holds the trigger state
	 * to be modified.
	 * @param Hand An enumeration specifying which hand's trigger functionality to
	 * disable (Left, Right, or AnyHand).
	 */
	inline void Off(FDeviceContext* Context, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x0;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x0;
		}
	}

	/**
	 * Configures resistance for the trigger functionality on the specified hand or
	 * hands in the given device context.
	 *
	 * @param Context A pointer to the device context that holds the trigger
	 * settings to be modified.
	 * @param StartZones The starting position of the resistance zone for the
	 * trigger.
	 * @param Strength The level of resistance to be applied within the defined
	 * zone.
	 * @param Hand An enumeration specifying which hand's trigger functionality to
	 * configure (Left, Right, or AnyHand).
	 */
	inline void Resistance(FDeviceContext* Context, std::uint8_t StartZones,
	                       std::uint8_t Strength, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x01;
			Context->Output.LeftTrigger.Strengths.Compose[0] = StartZones;
			Context->Output.LeftTrigger.Strengths.Compose[1] = Strength;
		}
		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x01;
			Context->Output.RightTrigger.Strengths.Compose[0] = StartZones;
			Context->Output.RightTrigger.Strengths.Compose[1] = Strength;
		}
	}

	/**
	 * Configures the trigger functionality on the specified hand or hands to
	 * emulate a GameCube-style resistance.
	 *
	 * @param Context A pointer to the device context that holds the trigger
	 * settings to be modified.
	 * @param Hand An enumeration specifying which hand's trigger functionality to
	 * configure (Left, Right, or AnyHand).
	 */
	inline void GameCube(FDeviceContext* Context, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x02;
			Context->Output.LeftTrigger.Strengths.Compose[0] = 0x90;
			Context->Output.LeftTrigger.Strengths.Compose[1] = 0x0a;
			Context->Output.LeftTrigger.Strengths.Compose[2] = 0xff;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x02;
			Context->Output.RightTrigger.Strengths.Compose[0] = 0x90;
			Context->Output.RightTrigger.Strengths.Compose[1] = 0x0a;
			Context->Output.RightTrigger.Strengths.Compose[2] = 0xff;
		}
	}

	inline void Bow22(FDeviceContext* Context, std::uint8_t StartZone,
	                  std::uint8_t SnapBack, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x22;
			Context->Output.LeftTrigger.Strengths.Compose[0] = StartZone;
			Context->Output.LeftTrigger.Strengths.Compose[1] = 0x01;
			Context->Output.LeftTrigger.Strengths.Compose[2] = SnapBack;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x22;
			Context->Output.RightTrigger.Strengths.Compose[0] = StartZone;
			Context->Output.RightTrigger.Strengths.Compose[1] = 0x01;
			Context->Output.RightTrigger.Strengths.Compose[2] = SnapBack;
		}
	}

	inline void Galloping23(FDeviceContext* Context, std::uint8_t StartPosition,
	                        std::uint8_t EndPosition, std::uint8_t FirstFoot,
	                        std::uint8_t SecondFoot, std::uint8_t Frequency,
	                        const EDSGamepadHand& Hand)
	{
		const std::uint8_t FirstFootNib = static_cast<std::uint8_t>(std::clamp(
		    static_cast<int>(std::lround((FirstFoot / 8) * 15)), 1, 15));
		const std::uint8_t SecondFootNib = static_cast<std::uint8_t>(std::clamp(
		    static_cast<int>(std::lround((SecondFoot / 8) * 15)), 1, 15));
		const std::uint16_t PositionMask = (1 << StartPosition) | (1 << EndPosition);
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x23;
			Context->Output.LeftTrigger.Strengths.Compose[0] = PositionMask & 0xFF;
			Context->Output.LeftTrigger.Strengths.Compose[1] =
			    (PositionMask >> 8) & 0xFF;
			Context->Output.LeftTrigger.Strengths.Compose[2] =
			    ((FirstFootNib & 0x0F) << 4) | (SecondFootNib & 0x0F);
			Context->Output.LeftTrigger.Strengths.Compose[3] = Frequency;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x23;
			Context->Output.RightTrigger.Strengths.Compose[0] = PositionMask & 0xFF;
			Context->Output.RightTrigger.Strengths.Compose[1] =
			    (PositionMask >> 8) & 0xFF;
			Context->Output.RightTrigger.Strengths.Compose[2] =
			    ((FirstFootNib & 0x0F) << 4) | (SecondFootNib & 0x0F);
			Context->Output.RightTrigger.Strengths.Compose[3] = Frequency;
		}
	}

	inline void Weapon25(FDeviceContext* Context, std::uint8_t StartZone,
	                     std::uint8_t Amplitude, std::uint8_t Behavior,
	                     std::uint8_t Trigger, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x25;
			Context->Output.LeftTrigger.Strengths.Compose[0] =
			    StartZone << 4 | (Amplitude & 0x0F);
			Context->Output.LeftTrigger.Strengths.Compose[1] = Behavior;
			Context->Output.LeftTrigger.Strengths.Compose[2] = Trigger & 0x0F;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x25;
			Context->Output.RightTrigger.Strengths.Compose[0] =
			    StartZone << 4 | (Amplitude & 0x0F);
			Context->Output.RightTrigger.Strengths.Compose[1] = Behavior;
			Context->Output.RightTrigger.Strengths.Compose[2] = Trigger & 0x0F;
		}
	}

	inline void MachineGun26(FDeviceContext* Context, std::uint8_t StartZone,
	                         std::uint8_t Behavior, std::uint8_t Amplitude,
	                         std::uint8_t Frequency, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x26;
			Context->Output.LeftTrigger.Strengths.Compose[0] = 0xf8;
			Context->Output.LeftTrigger.Strengths.Compose[1] =
			    Behavior > 0 ? 0x03 : 0x00;
			Context->Output.LeftTrigger.Strengths.Compose[2] = 0x00;
			Context->Output.LeftTrigger.Strengths.Compose[3] = 0x00;
			Context->Output.LeftTrigger.Strengths.Compose[4] =
			    Amplitude == 1 ? 0x8F : 0x8a;
			Context->Output.LeftTrigger.Strengths.Compose[5] =
			    Amplitude == 2 ? 0x3F : 0x1F;
			Context->Output.LeftTrigger.Strengths.Compose[9] = Frequency;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x26;
			Context->Output.RightTrigger.Strengths.Compose[0] = 0xf8;
			Context->Output.RightTrigger.Strengths.Compose[1] =
			    Behavior > 0 ? 0x03 : 0x00;
			Context->Output.RightTrigger.Strengths.Compose[2] = 0x00;
			Context->Output.RightTrigger.Strengths.Compose[3] = 0x00;
			Context->Output.RightTrigger.Strengths.Compose[4] =
			    Amplitude == 1 ? 0x8F : 0x8a;
			Context->Output.RightTrigger.Strengths.Compose[5] =
			    Amplitude == 2 ? 0x3F : 0x1F;
			Context->Output.RightTrigger.Strengths.Compose[9] = Frequency;
		}
	}

	inline void Machine27(FDeviceContext* Context, std::uint8_t StartZone,
	                      std::uint8_t BehaviorFlag, std::uint8_t Force,
	                      std::uint8_t Amplitude, std::uint8_t Period,
	                      std::uint8_t Frequency, const EDSGamepadHand& Hand)
	{
		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0x27;
			Context->Output.LeftTrigger.Strengths.Compose[0] = StartZone;
			Context->Output.LeftTrigger.Strengths.Compose[1] =
			    BehaviorFlag > 0 ? 0x02 : 0x01;
			Context->Output.LeftTrigger.Strengths.Compose[2] =
			    Force << 4 | (Amplitude & 0x0F);
			;
			Context->Output.LeftTrigger.Strengths.Compose[3] = Period;
			Context->Output.LeftTrigger.Strengths.Compose[4] = Frequency;
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0x27;
			Context->Output.RightTrigger.Strengths.Compose[0] = StartZone;
			Context->Output.RightTrigger.Strengths.Compose[1] =
			    BehaviorFlag > 0 ? 0x02 : 0x00;
			Context->Output.RightTrigger.Strengths.Compose[2] =
			    Force << 4 | (Amplitude & 0x0F);
			Context->Output.RightTrigger.Strengths.Compose[3] = Period;
			Context->Output.RightTrigger.Strengths.Compose[4] = Frequency;
		}
	}

	inline void CustomTrigger(FDeviceContext* Context, const EDSGamepadHand& Hand,
	                          const std::vector<std::uint8_t>& HexBytes)
	{
		switch (HexBytes[0])
		{
			case 0x01:
			case 0x02:
			case 0x21:
			case 0x22:
			case 0x23:
			case 0x25:
			case 0x26:
			case 0x27:
				break;
			default:
				return;
		}

		if (Hand == EDSGamepadHand::Left || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.LeftTrigger.Mode = 0xFF;
			std::memcpy(Context->Output.LeftTrigger.Strengths.Compose, HexBytes.data(), 10);
		}

		if (Hand == EDSGamepadHand::Right || Hand == EDSGamepadHand::AnyHand)
		{
			Context->Output.RightTrigger.Mode = 0xFF;
			std::memcpy(Context->Output.RightTrigger.Strengths.Compose, HexBytes.data(), 10);
		}
	}

} // namespace FDualSenseTriggerComposer
