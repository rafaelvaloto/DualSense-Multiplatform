// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Interfaces/Segregations/IGamepadAudioHaptics.h"
#include "Core/Interfaces/Segregations/IGamepadTrigger.h"
#include "Core/Types/DSCoreTypes.h"
#include "Core/Types/ECoreGamepad.h"
#include "Core/Types/Structs/Context/DeviceContext.h"
#include "Implementations/Libraries/Base/SonyGamepadAbstract.h"

/**
 * @class FDualSenseLibrary
 * @brief Utility class for interfacing with the PlayStation DualSense
 * controller.
 *
 * This class provides a collection of static functions and utilities specific
 * to interacting with the PlayStation DualSense controller within a project. It
 * facilitates operations such as managing haptic feedback, adaptive triggers,
 * and other special features offered by the controller.
 *
 * The UDualSenseLibrary is typically used to control and respond to events from
 * a connected DualSense device, enabling advanced functionalities for an
 * enhanced user experience.
 *
 * Features supported may include:
 * - Triggering haptic feedback.
 * - Configuring adaptive trigger effects.
 * - Managing light bars and other controller-specific outputs.
 * - Retrieving connected-state and other metadata.
 *
 * The library is designed for developers seeking to leverage unique features of
 * the DualSense controller programmatically within an application.
 */
class FDualSenseLibrary : public SonyGamepadAbstract,
                          public IGamepadTrigger,
                          public IGamepadAudioHaptics
{

public:
	FDualSenseLibrary()
	    : AudioVibrationSequence(0)
	{}

	/**
	 * @brief Retrieves the current gamepad trigger implementation.
	 *
	 * Returns a pointer to the current instance implementing the IGamepadTrigger
	 * interface. This method allows access to the gamepad trigger behavior and
	 * state.
	 *
	 * @return A pointer to the IGamepadTrigger implementing object, typically
	 * this instance.
	 */
	virtual IGamepadTrigger* GetIGamepadTrigger() override { return this; }

	/**
	 * @brief Retrieves the interface for handling gamepad audio haptics.
	 *
	 * This method returns a pointer to an object that implements the
	 * IGamepadAudioHaptics interface, which provides functionalities related to
	 * the audio haptics of a gamepad.
	 *
	 * @return A pointer to the IGamepadAudioHaptics instance.
	 */
	virtual IGamepadAudioHaptics* GetIGamepadHaptics() override { return this; }

	/**
	 * @brief Configures settings for the PlayStation DualSense controller.
	 *
	 * This method allows the configuration of various audio and controller features,
	 * such as the microphone, headset, speaker, volume levels, and vibration settings.
	 * It is typically used to apply specific settings to enhance or modify the user's
	 * gameplay experience based on the connected DualSense controller's capabilities.
	 *
	 * @param bIsMic Indicates whether the microphone is enabled (1) or disabled (0).
	 * @param bIsHeadset Indicates whether the headset is connected (1) or not (0).
	 * @param bIsSpeaker Indicates whether the speaker is enabled (1) or disabled (0).
	 * @param MicVolume Specifies the microphone volume level (0–255).
	 * @param AudioVolume Specifies the audio volume level for the headset or speaker (0–255).
	 * @param RumbleMode Configures the rumble/vibration mode (value range is implementation-specific).
	 * @param RumbleReduce Reduces the intensity of the rumble effect (value range is implementation-specific).
	 * @param TriggerReduce Adjusts the sensitivity of the adaptive triggers (value range is implementation-specific).
	 */
	virtual void DualSenseSettings(std::uint8_t bIsMic, std::uint8_t bIsHeadset, std::uint8_t bIsSpeaker, std::uint8_t MicVolume, std::uint8_t AudioVolume, std::uint8_t RumbleMode, std::uint8_t RumbleReduce, std::uint8_t TriggerReduce) override;

	/**
	 * @brief Updates the output state of the gamepad.
	 *
	 * This method is responsible for refreshing or modifying the output-related
	 * functionality of the Sony gamepad. It overrides the UpdateOutput method
	 * defined in the ISonyGamepad interface and provides a customized
	 * implementation specific to derived classes.
	 *
	 * The UpdateOutput functionality may include updating lightbar states,
	 * haptic feedback, audio output, or other gamepad output features based on
	 * the current state or input from the system.
	 */
	virtual void UpdateOutput() override;
	/**
	 * @brief Initializes the DualSense library with the specified device context.
	 *
	 * This method sets up the library with the provided device context,
	 * configuring the necessary states for the DualSense controller, such as
	 * enabling or disabling certain features. It ensures the library is prepared
	 * to handle input from a connected DualSense device.
	 *
	 * @param Context A reference to an FDeviceContext structure that contains
	 * information about the current device, such as the connection type and
	 * configuration.
	 *
	 * @return Returns true if the library was successfully initialized.
	 */
	virtual bool Initialize(const FDeviceContext& Context) override;
	/**
	 * @brief Updates the input state for a DualSense device.
	 *
	 * This method processes input data received from a DualSense controller and
	 * updates the application's state via the provided message handler. It
	 * ensures that the input is correctly mapped and associated with the
	 * specified platform user and input device.
	 * @return A boolean value indicating whether the input update was successful.
	 */
	virtual void UpdateInput(float Delta) override;
	/**
	 * Stops any ongoing adaptive trigger effects on the specified controller
	 * hand.
	 *
	 * @param Hand The hand for which to stop the adaptive trigger effect.
	 *             Acceptable values are EDSGamepadHand::Left,
	 * EDSGamepadHand::Right, or EDSGamepadHand::AnyHand.
	 */
	virtual void StopTrigger(const EDSGamepadHand& Hand) override;
	/**
	 * @brief Configures the trigger settings on a DualSense controller for
	 * GameCube-style behavior.
	 *
	 * This method configures the triggers on the DualSense controller to emulate
	 * a GameCube-style response, based on the specified controller hand.
	 * It adjusts the trigger's mode and strengths for the desired effects.
	 *
	 * @param Hand The hand (left, right, or both) corresponding to the controller
	 * side where the GameCube-style trigger behavior should be applied. Must be
	 * a value of the EDSGamepadHand enumeration.
	 */
	virtual void SetGameCube(const EDSGamepadHand& Hand) override;
	/**
	 * @brief Sets the trigger resistance properties for a specific controller
	 * hand.
	 *
	 * This method configures the adaptive trigger resistance on a controller,
	 * specifying the starting zones and strength of the resistance based on input
	 * parameters. It applies the settings to the designated controller hand,
	 * allowing for precise customization of trigger feedback in gameplay or
	 * applications.
	 *
	 * @param StartZones Specifies the starting zones in which the resistance
	 * should be applied.
	 * @param Strength Determines the intensity or strength of the resistance.
	 * @param Hand Indicates the controller hand (e.g., left or right) where the
	 * resistance should be configured.
	 */
	virtual void SetResistance(std::uint8_t StartZones, std::uint8_t Strength,
	                           const EDSGamepadHand& Hand) override;
	/**
	 * @brief Configures the bow effect settings on a DualSense controller.
	 *
	 * This method allows customization of the bow effect by specifying its start
	 * zone, behavior, force amplitude, and the hand associated with the action.
	 *
	 * @param StartZone The starting zone value for the bow effect.
	 * @param SnapBack The SnapBack of the force applied during the bow effect.
	 * @param Hand The controller hand (left or right) associated with the bow
	 * action.
	 */
	virtual void SetBow22(std::uint8_t StartZone, std::uint8_t SnapBack,
	                      const EDSGamepadHand& Hand) override;
	/**
	 * @brief Configures the galloping trigger feedback behavior on a DualSense
	 * controller.
	 *
	 * This method sets up the trigger effects on the specified hand of a
	 * DualSense controller to emulate a "galloping" feedback pattern. It allows
	 * customization of start and end positions, the characteristics of two
	 * feedback "foot" pressures, and the frequency of the effect.
	 *
	 * @param StartPosition The beginning position of the galloping effect,
	 * represented as a 0-based index.
	 * @param EndPosition The ending position of the galloping effect, represented
	 * as a 0-based index.
	 * @param FirstFoot The feedback strength value for the first foot during the
	 * galloping effect, ranging from 0-8.
	 * @param SecondFoot The feedback strength value for the second foot during
	 * the galloping effect, ranging from 0-8.
	 * @param Frequency The frequency of the galloping effect, determining how
	 * rapidly it alternates or triggers.
	 * @param Hand Specifies the controller hand (left, right, or any) to apply
	 * the galloping effect.
	 */
	virtual void SetGalloping23(std::uint8_t StartPosition,
	                            std::uint8_t EndPosition, std::uint8_t FirstFoot,
	                            std::uint8_t SecondFoot, std::uint8_t Frequency,
	                            const EDSGamepadHand& Hand) override;
	/**
	 * @brief Configures the trigger effect for the DualSense controller's
	 * adaptive triggers.
	 *
	 * This method sets the advanced bow effect (Mode 0x25) for the specified
	 * controller triggers, allowing customization of trigger resistance and
	 * behavior for enhanced gaming experiences.
	 *
	 * @param StartZone Specifies the starting position of the trigger effect.
	 * @param Amplitude Defines the amplitude or intensity of the resistance
	 * effect.
	 * @param Behavior Determines the behavior of the effect in terms of
	 * responsiveness and resistance.
	 * @param Trigger Specifies an additional parameter for customizing the effect
	 * behavior.
	 * @param Hand Determines which hand (Left, Right, or AnyHand) the
	 * configuration applies to.
	 */
	virtual void SetWeapon25(std::uint8_t StartZone, std::uint8_t Amplitude,
	                         std::uint8_t Behavior, std::uint8_t Trigger,
	                         const EDSGamepadHand& Hand) override;
	/**
	 * @brief Configures the machine gun effect on a DualSense controller.
	 *
	 * This method controls the haptic feedback pattern to simulate a
	 * machine-gun-like vibration effect on the controller. It allows setting
	 * parameters such as the starting zone, behavior, amplitude, frequency, and
	 * the specific controller hand.
	 *
	 * @details The method enables developers to provide a highly immersive
	 * experience by fine-tuning the haptic responses. By adjusting the starting
	 * zone and behavior, the effect can be customized to simulate different
	 * mechanical sensations, while the amplitude and frequency control the
	 * intensity and oscillation of the feedback.
	 *
	 * @param StartZone Specifies the initial zone for the haptic effect.
	 * @param Behavior Defines the behavioral pattern of the machine gun effect.
	 * @param Amplitude Sets the vibration amplitude, determining the strength of
	 * the feedback.
	 * @param Frequency Configures the frequency of the haptic pulses to simulate
	 * firing intervals.
	 * @param Hand Indicates which controller hand (left or right) will receive
	 * the effect.
	 */
	virtual void SetMachineGun26(std::uint8_t StartZone, std::uint8_t Behavior,
	                             std::uint8_t Amplitude, std::uint8_t Frequency,
	                             const EDSGamepadHand& Hand) override;
	/**
	 * @brief Configures the advanced machine effect (Mode 0x27) for DualSense
	 * controller triggers.
	 *
	 * This function sets the parameters for the Mode 0x27 advanced effect on the
	 * adaptive triggers of a DualSense controller. It specifies the behavior of
	 * the effect, including starting zone, force, amplitude, period, and
	 * frequency, and applies these configurations to the left, right, or both
	 * triggers, based on the specified hand.
	 *
	 * @param StartZone Specifies the starting zone of the trigger effect. Defines
	 * the point where the effect begins.
	 * @param BehaviorFlag Indicates the behavior type of the trigger effect.
	 * Determines specific effect adjustments (e.g., feedback type).
	 * @param Force Defines the level of force to apply in the trigger effect.
	 * Higher values result in stronger feedback.
	 * @param Amplitude Sets the amplitude of the trigger vibration or feedback
	 * pattern.
	 * @param Period Specifies the period for the trigger effect, controlling
	 * timing intervals or pulse duration.
	 * @param Frequency Determines the frequency for the trigger vibration or
	 * repeated effect.
	 * @param Hand Identifies the controller hand (left, right, or both) for
	 * applying the trigger effect.
	 */
	virtual void SetMachine27(std::uint8_t StartZone, std::uint8_t BehaviorFlag,
	                          std::uint8_t Force, std::uint8_t Amplitude,
	                          std::uint8_t Period, std::uint8_t Frequency,
	                          const EDSGamepadHand& Hand) override;
	/**
	 * @brief Sets custom trigger behavior for the specified controller hand using
	 * custom hexadecimal byte data.
	 *
	 * This method allows developers to define custom behaviors for the trigger
	 * buttons on a DualSense controller. The behavior is specified using an array
	 * of 10 hexadecimal byte strings which represent configuration and behavior
	 * codes that are sent directly to the controller.
	 *
	 * @param Hand The hand designation of the controller (e.g., left, right, or
	 * any hand) for which the custom trigger behavior is applied.
	 * @param HexBytes An array of hexadecimal byte strings defining the custom
	 * trigger configuration. Must contain exactly 10 valid values.
	 */
	virtual void
	SetCustomTrigger(const EDSGamepadHand& Hand,
	                 const std::vector<std::uint8_t>& HexBytes) override;

	/**
	 * Sets the LED player indicator effects based on the desired player LED
	 * pattern and brightness intensity.
	 *
	 * @param Led The LED pattern to be displayed on the device, represented by
	 * the ELedPlayerEnum enumeration.
	 * @param Brightness The brightness intensity for the LED, represented by the
	 * ELedBrightnessEnum enumeration.
	 */
	virtual void SetPlayerLed(EDSPlayer Led, std::uint8_t Brightness) override;
	/**
	 * Sets the microphone LED effects on the DualSense controller.
	 *
	 * @param Led The desired LED microphone effect to be applied. It is defined
	 * by the ELedMicEnum enumeration, which includes options such as MicOff,
	 * MicOn, and Pulse.
	 */
	virtual void SetMicrophoneLed(EDSMic Led) override;
	/**
	 * @brief Sets the vibration strength for the DualSense controller.
	 *
	 * This function allows adjustment of the vibration motors in the controller,
	 * enabling haptic feedback customization. The intensity of the vibration can
	 * be independently set for the left and right motors.
	 *
	 * @param LeftRumble The intensity of the left motor's vibration (0-255).
	 * Optional, defaults to 0.
	 * @param RightRumble The intensity of the right motor's vibration (0-255).
	 * Optional, defaults to 0.
	 */
	virtual void SetVibration(std::uint8_t LeftRumble = 0,
	                          std::uint8_t RightRumble = 0) override;
	/**
	 * @brief Stops all ongoing input and feedback operations on the DualSense
	 * controller.
	 *
	 * The StopAll function halts all active feedback features such as LED lights,
	 * trigger feedback, and other customizable components on the DualSense
	 * controller. It resets the state of the controller's visual and haptic
	 * features.
	 *
	 * @details This function modifies the controller's state by turning off the
	 * brightness of the player LED, deactivating lightbar and trigger modes, and
	 * updating the lightbar colors based on the assigned ControllerID. Each
	 * ControllerID is associated with a specific lightbar color to represent
	 * different controller states.
	 *
	 * The function ensures consistent controller deactivation for any connected
	 * players by resetting features to their default state and immediately
	 * sending the state update to the hardware. This can be used in scenarios
	 * where the controller must be brought into a neutral state, such as when
	 * pausing gameplay or shutting down the system.
	 */
	virtual void ResetLights() override;
	/**
	 * @brief Sets the lightbar color and updates its behavior on a DualSense
	 * controller.
	 *
	 * This method allows customization of the DualSense controller's lightbar by
	 * setting its color and configuring the timing for brightness and toggle
	 * transitions. It communicates directly with the device context to apply the
	 * desired changes.
	 *
	 * @param Color The desired color of the lightbar, represented as an FDSColor
	 */
	virtual void SetLightbar(DSCoreTypes::FDSColor Color) override;
	virtual void SetLightbarFlash(DSCoreTypes::FDSColor Color, float BrithnessTime, float ToggleTime) override {};
	/**
	 * @brief Updates the haptic feedback system of the DualSense controller with
	 * audio data.
	 *
	 * The AudioHapticUpdate method sends audio-based haptic feedback data to the
	 * DualSense controller. It handles encoding and transmitting audio data to
	 * produce haptic vibration effects on the controller hardware.
	 *
	 * @param Data A byte array containing the audio haptic data to be
	 * transmitted. The method processes up to a maximum of 64 bytes of this data.
	 *
	 * @details This function interacts with the device context to check if the
	 * controller is connected, processes the provided audio data into the
	 * appropriate format, and forwards it to the DualSense hardware. The method
	 * ensures memory-safe data handling and integrates with advanced
	 * communication protocols to achieve synchronized haptic feedback.
	 *
	 * This functionality is typically implemented in systems that aim to provide
	 * immersive feedback during audio playback or gaming scenarios that utilize
	 * DualSense controllers.
	 */
	virtual void AudioHapticUpdate(std::vector<std::uint8_t> Data) override;

private:
	/**
	 * @variable AudioVibrationSequence
	 * @brief Represents the identifier for a sequence of audio-guided vibrations.
	 *
	 * This variable is typically used to define or handle a specific pattern of
	 * vibrations that correspond to audio feedback. It serves as a reference to a
	 * predefined or dynamically generated vibration sequence that can synchronize
	 * with audio events for immersive experiences.
	 *
	 * @details The value stored in this variable is used to index or
	 * differentiate between multiple vibration sequences. It allows systems to
	 * execute specific vibration patterns that align with corresponding audio
	 * playback, enhancing features such as haptic feedback in interactive
	 * environments or devices.
	 */
	std::uint8_t AudioVibrationSequence;
};
