// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

class IGamepadLightbar
{
public:
	virtual ~IGamepadLightbar() = default;
	/**
	 * Sets the lightbar flash color and associated timing parameters on the gamepad.
	 *
	 * @param Color The color to set the lightbar to, represented as an FColor.
	 * @param BrithnessTime The time duration for setting the brightness effect.
	 * Defaults to 0.0f.
	 * @param ToggleTime The time interval for toggling the lightbar. Defaults to
	 * 0.0f.
	 */
	virtual void SetLightbarFlash(DSCoreTypes::FDSColor Color, float BrithnessTime, float ToggleTime) = 0;
	/**
	 * Sets the lightbar flash color and associated timing parameters on the gamepad.
	 *
	 * @param Color The color to set the lightbar to, represented as an FColor.
	 */
	virtual void SetLightbar(DSCoreTypes::FDSColor Color) = 0;
	/**
	 * Sets the LED associated with the player on the Sony gamepad to a specified
	 * brightness level.
	 *
	 * @param Led The LED associated with a specific player, represented as an
	 * EDSPlayer.
	 * @param Brightness The desired brightness level for the LED.
	 */
	virtual void SetPlayerLed(EDSPlayer Led, std::uint8_t Brightness) = 0;
	/**
	 * Sets the state of the microphone LED on the gamepad.
	 *
	 * @param Led The desired state of the microphone LED, represented by
	 * ELedMicEnum.
	 */
	virtual void SetMicrophoneLed(EDSMic Led) = 0;
	/**
	 * Stops all currently active operations or actions associated with the
	 * interface. This method must be implemented by any derived class to handle
	 * the termination of all ongoing processes.
	 */
	virtual void ResetLights() = 0;
};
