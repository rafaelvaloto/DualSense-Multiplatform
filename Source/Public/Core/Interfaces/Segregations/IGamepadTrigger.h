// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Types/DSCoreTypes.h"

/**
 *
 */
class IGamepadTrigger
{
	// Add interface functions to this class. This is the class that will be
	// inherited to implement this interface.
public:
	virtual ~IGamepadTrigger() = default;
	/**
	 * Disables the trigger effects on a gamepad for the specified controller
	 * hand.
	 *
	 * @param Hand An enum indicating which controller hand's trigger effects
	 * should be stopped.
	 */
	virtual void StopTrigger(const EDSGamepadHand& Hand) = 0;
	/**
	 * Configures the GameCube controller interface for a specified controller
	 * hand.
	 *
	 * @param Hand The controller hand (left or right) for which the GameCube
	 * interface configuration is applied.
	 */
	virtual void SetGameCube(const EDSGamepadHand& Hand) = 0;
	/**
	 * Configures the resistance level on a gamepad's trigger for the specified
	 * controller hand.
	 *
	 * @param StartZones Specifies the starting zones on the trigger where
	 * resistance is applied.
	 * @param Strength The intensity of the resistance effect.
	 * @param Hand An enum identifying the controller hand to configure.
	 */
	virtual void SetResistance(std::uint8_t StartZones, std::uint8_t Strength,
	                           const EDSGamepadHand& Hand) = 0;
	/**
	 * Activates custom trigger configurations on a gamepad for a specified hand.
	 *
	 * @param Hand A reference to the EDSGamepadHand enum specifying the hand
	 *             (e.g., left or right) for which the trigger is being
	 * configured.
	 * @param HexBytes An array of strings representing hexadecimal values that
	 * define the trigger configuration parameters.
	 */
	virtual void SetCustomTrigger(const EDSGamepadHand& Hand,
	                              const std::vector<std::uint8_t>& HexBytes) = 0;
	/**
	 * Configures the bow tension effect on the gamepad triggers based on
	 * specified parameters.
	 *
	 * @param StartZone The start position of the effect's activation zone on the
	 * trigger.
	 * @param SnapBack The intensity or force of the bow effect applied to the
	 * trigger.
	 * @param Hand The controller hand (left or right) to which the effect should
	 * be applied.
	 */
	virtual void SetBow22(std::uint8_t StartZone, std::uint8_t SnapBack,
	                      const EDSGamepadHand& Hand) = 0;
	/**
	 * Configures a galloping effect on the gamepad with specified parameters for
	 * position, foot rhythm, and frequency.
	 *
	 * @param StartPosition The starting position of the galloping effect.
	 * @param EndPosition The ending position of the galloping effect.
	 * @param FirstFoot The force or intensity value assigned to the first foot.
	 * @param SecondFoot The force or intensity value assigned to the second foot.
	 * @param Frequency The frequency of the galloping effect.
	 * @param Hand The specific controller hand to which the effect is applied.
	 */
	virtual void SetGalloping23(std::uint8_t StartPosition,
	                            std::uint8_t EndPosition, std::uint8_t FirstFoot,
	                            std::uint8_t SecondFoot, std::uint8_t Frequency,
	                            const EDSGamepadHand& Hand) = 0;
	/**
	 * Configures trigger behavior for the specified weapon mode on a gamepad.
	 *
	 * @param StartZone The starting position of the trigger's actuation zone.
	 * @param Amplitude The intensity of the trigger's feedback effect.
	 * @param Behavior The type of feedback behavior applied to the trigger.
	 * @param Trigger The specific trigger to configure.
	 * @param Hand The controller hand (left or right) associated with the
	 * trigger.
	 */
	virtual void SetWeapon25(std::uint8_t StartZone, std::uint8_t Amplitude,
	                         std::uint8_t Behavior, std::uint8_t Trigger,
	                         const EDSGamepadHand& Hand) = 0;
	/**
	 * Configures the machine gun effect on the gamepad triggers with specific
	 * parameters.
	 *
	 * @param StartZone The starting position of the trigger's activation zone for
	 * the machine gun effect.
	 * @param Behavior The behavior pattern or mode applied to the trigger.
	 * @param Amplitude The intensity level of the feedback effect on the trigger.
	 * @param Frequency The frequency of the feedback effect for the machine gun
	 * effect.
	 * @param Hand The controller hand (left or right) to which the machine gun
	 * effect is applied.
	 */
	virtual void SetMachineGun26(std::uint8_t StartZone, std::uint8_t Behavior,
	                             std::uint8_t Amplitude, std::uint8_t Frequency,
	                             const EDSGamepadHand& Hand) = 0;
	/**
	 * Configures the behavior and haptic effects for a specific machine profile
	 * on a gamepad.
	 *
	 * @param StartZone The starting zone value indicating the trigger's
	 * activation point.
	 * @param BehaviorFlag A flag that determines the behavior pattern
	 * configuration for the trigger.
	 * @param Force The amount of force applied to the trigger feedback mechanism.
	 * @param Amplitude The amplitude level for the feedback intensity.
	 * @param Period The duration of the haptic feedback signal in each cycle.
	 * @param Frequency The frequency of the haptic feedback signal.
	 * @param Hand The hand (left or right) to which the feedback effect will be
	 * applied, based on the EDSGamepadHand enumeration.
	 */
	virtual void SetMachine27(std::uint8_t StartZone, std::uint8_t BehaviorFlag,
	                          std::uint8_t Force, std::uint8_t Amplitude,
	                          std::uint8_t Period, std::uint8_t Frequency,
	                          const EDSGamepadHand& Hand) = 0;
};
