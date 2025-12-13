// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Algorithms/MadgwickAhrs.h"
#include "Core/Interfaces/ISonyGamepad.h"
#include "Core/Types/DSCoreTypes.h"
#include "Core/Types/ECoreGamepad.h"

/**
 * @class SonyGamepadAbstract
 * @brief An abstract base class derived from the ISonyGamepad interface.
 *
 * The SonyGamepadAbstract class serves as a foundational class for implementing
 * gamepad functionality specific to Sony devices. It inherits from the
 * ISonyGamepad interface, which provides methods and features for managing
 * gamepad status, lightbar, sensors, touch input, triggers, audio-driven
 * haptics, and more.
 *
 * Classes inheriting from SonyGamepadAbstract are expected to implement the
 * behaviors defined by the ISonyGamepad interface.
 */
class SonyGamepadAbstract : public ISonyGamepad
{

public:
	/**
	 * @brief Shuts down the library and releases associated resources.
	 *
	 * This method is invoked to perform cleanup tasks, ensuring that all
	 * resources allocated by the library are properly released. It overrides the
	 * equivalent method in the ISonyGamepad interface and ensures the integrity
	 * of the shutdown process specific to Sony gamepad implementations.
	 */
	void ShutdownLibrary() override;
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
	void UpdateOutput() override {}
	/**
	 * @brief Updates the input state of the gamepad.
	 *
	 * The UpdateInput method is called to process and refresh the input data
	 * associated with the gamepad. This function is typically invoked on each
	 * frame with the elapsed time (Delta) since the last update, allowing input
	 * state to be updated in real-time.
	 *
	 * @param Delta The time elapsed since the previous frame, in seconds.
	 *
	 * @details This method is meant to be overridden to implement specific
	 * behaviors for handling input updates in derived classes. It ensures that
	 * the gamepad's state is continuously synchronized with the current input
	 * conditions, supporting real-time interaction within the application.
	 */
	void UpdateInput(float Delta) override {}
	/**
	 * Pure virtual function that checks the connection status of the gamepad.
	 *
	 * @return True if the gamepad is connected, false otherwise.
	 */
	bool IsConnected() override;
	/**
	 * Retrieves the type of the device.
	 *
	 * @return The type of the device as an EDeviceType enumeration.
	 */
	EDSDeviceType GetDeviceType() override;
	/**
	 * Retrieves the connection type of the device.
	 *
	 * @return The connection type of the device as an EDeviceConnection
	 * enumeration.
	 */
	EDSDeviceConnection GetConnectionType() override;
	/**
	 * Retrieves the current battery level of the Sony gamepad.
	 *
	 * @return The battery level as a floating-point value, where the exact range
	 *         and representation are dependent on the implementation. Typically,
	 *         values may range between 0.0 (empty) and 1.0 (full).
	 */
	float GetBattery() override;
	/**
	 * Stops all currently active operations or actions associated with the
	 * interface. This method must be implemented by any derived class to handle
	 * the termination of all ongoing processes.
	 */
	void ResetLights() override {}
	void SetLightbar(DSCoreTypes::FDSColor Color) override {}
	void SetLightbarFlash(DSCoreTypes::FDSColor Color, float BrithnessTime, float ToggleTime) override {}
	void DualSenseSettings(std::uint8_t bIsMic, std::uint8_t bIsHeadset, std::uint8_t bIsSpeaker, std::uint8_t MicVolume, std::uint8_t AudioVolume, std::uint8_t RumbleMode, std::uint8_t RumbleReduce, std::uint8_t TriggerReduce) override {}

	/**
	 * Sets the LED associated with the player on the Sony gamepad to a specified
	 * brightness level.
	 *
	 * @param Led The LED associated with a specific player, represented as an
	 * ELedPlayerEnum.
	 * @param Brightness The desired brightness level for the LED, represented as
	 * an ELedBrightnessEnum.
	 */
	void SetPlayerLed(EDSPlayer Led, std::uint8_t Brightness) override {}
	/**
	 * Sets the state of the microphone LED on the gamepad.
	 *
	 * @param Led The desired state of the microphone LED, represented by
	 * ELedMicEnum.
	 */
	void SetMicrophoneLed(EDSMic Led) override {}
	/**
	 * Sets the touch state for the device.
	 *
	 * @param bIsTouch A boolean indicating whether touch input is enabled (true)
	 * or disabled (false).
	 */
	void EnableTouch(bool bIsTouch) override;
	/**
	 * Sets the touch state for the device.
	 *
	 * @param bIsGesture A boolean indicating whether touch input is enabled
	 * (true) or disabled (false).
	 */
	void EnableGesture(bool bIsGesture) override;
	/**
	 * Resets the orientation of the gyroscope to its default state.
	 * Typically used to recalibrate the gyroscope sensor.
	 */
	void ResetGyroOrientation() override;
	/**
	 * Enables the motion sensor functionality of the gamepad.
	 *
	 * @param bIsMotionSensor Specifies whether to enable the gyroscope (true) or
	 * accelerometer (false) as the motion sensor.
	 */
	void EnableMotionSensor(bool bIsMotionSensor) override;
	/**
	 * @brief Retrieves a mutable device context associated with the object.
	 *
	 * This method provides access to the device context in a mutable form,
	 * allowing modifications to the underlying hardware interface descriptor
	 * (HID) data specific to the device.
	 *
	 * @return A pointer to the mutable FDeviceContext object corresponding to the
	 * device's HID context.
	 */
	FDeviceContext* GetMutableDeviceContext() override
	{
		return &HIDDeviceContexts;
	}

	/**
	 * @brief Sets the vibration feedback of the gamepad.
	 *
	 * This method overrides the base implementation to configure the vibration
	 * feedback intensity for the gamepad based on the provided force feedback
	 * values.
	 *
	 */
	void SetVibration(std::uint8_t LeftRumble, std::uint8_t RightRumble) override {}

	/**
	 * @class FMadgwickAhrs
	 * @brief Implements the Madgwick filter algorithm for sensor fusion in
	 * orientation tracking.
	 *
	 * The FMadgwickAhrs class is designed to process data from an IMU (Inertial
	 * Measurement Unit), applying the Madgwick filter algorithm to estimate
	 * orientation (roll, pitch, and yaw). It fuses accelerometer, gyroscope, and
	 * optionally magnetometer data to provide reliable orientation estimations.
	 *
	 * This class is commonly utilized in applications requiring real-time
	 * orientation tracking, such as motion sensing, virtual reality systems, and
	 * robotics.
	 *
	 * @details The algorithm is specifically optimized for low computational
	 * overhead while maintaining accurate results. It is well-suited for embedded
	 * systems or devices with constrained processing resources. Using this class,
	 * applications can achieve smooth and responsive 6-DOF (Degrees of Freedom)
	 * motion and orientation tracking.
	 */
	FMadgwickAhrs MadgwickFilter;

	SonyGamepadAbstract()
	    : MadgwickFilter(0.8)
	    , bEnableTouch(false)
	    , bEnableGesture(false)
	    , bHasPhoneConnected(false)
	    , BatteryLevel(0)
	    , bEnableAccelerometerAndGyroscope(false)
	    , HIDDeviceContexts()
	{}

protected:
	[[nodiscard]] bool IsEnableGesture() const { return bEnableGesture; }
	[[nodiscard]] bool IsEnableTouch() const { return bEnableTouch; }
	[[nodiscard]] bool IsEnableAccelerometerAndGyroscope() const
	{
		return bEnableAccelerometerAndGyroscope;
	}
	[[nodiscard]] bool IsResetGyroscope() const { return bIsResetGyroscope; }
	void SetIsResetGyroscope(const bool IsResetGyroscope)
	{
		this->bIsResetGyroscope = IsResetGyroscope;
	}

	/**
	 * @brief Sets the device contexts for the HID device.
	 *
	 * This method assigns the provided device contexts to the internal HID device
	 * contexts, enabling the configuration or update of the underlying device
	 * settings.
	 *
	 * @param DeviceContexts A reference to the FDeviceContext object representing
	 * the new HID device contexts.
	 */
	void SetDeviceContexts(const FDeviceContext& DeviceContexts)
	{
		this->HIDDeviceContexts = DeviceContexts;
	}

private:
	/**
	 * @brief A variable that indicates whether touch functionality is enabled or
	 * disabled.
	 *
	 * This variable is used to toggle the touch input capability of the system or
	 * application. When set to true, touch input is enabled, allowing the system
	 * to respond to touch events. When set to false, touch input is disabled, and
	 * touch interactions are ignored.
	 */
	bool bEnableTouch;
	/**
	 * @brief A variable that indicates whether touch functionality is enabled or
	 * disabled.
	 *
	 * This variable is used to toggle the touch input capability of the system or
	 * application. When set to true, touch input is enabled, allowing the system
	 * to respond to touch events. When set to false, touch input is disabled, and
	 * touch interactions are ignored.
	 */
	bool bEnableGesture;
	/**
	 * Indicates whether a phone is connected to the system.
	 *
	 * This variable is used to store the connection status of a phone.
	 * It is updated by the SetHasPhoneConnected method.
	 * - `true`: A phone is connected.
	 * - `false`: No phone is connected.
	 */
	bool bHasPhoneConnected;
	/**
	 * @brief Represents the current level of the battery.
	 *
	 * This variable is used to store the battery percentage or charge level,
	 * typically as an integer or floating-point value. It can be used to
	 * monitor battery status in various applications and scenarios.
	 *
	 * Note: The scale and representation (e.g., percentage, voltage) should
	 * be specified in the implementation or context where the variable is used.
	 */
	float BatteryLevel;
	/**
	 * @variable EnableAccelerometerAndGyroscope
	 * @brief Flags the activation of accelerometer and gyroscope sensors in the
	 * system.
	 *
	 * This variable determines whether the accelerometer and gyroscope
	 * functionalities are enabled for the system. When set to true, data from
	 * these sensors will be collected and utilized, typically for motion
	 * detection or orientation tracking.
	 *
	 * @details This flag is often used in systems that require motion input for
	 * functionality, such as gaming controllers, virtual reality devices, or
	 * motion-sensing applications. Disabling this may reduce resource usage but
	 * will disable motion-based features.
	 */
	bool bEnableAccelerometerAndGyroscope;
	/**
	 * @var bIsResetGyroscope
	 * @brief Indicates whether the gyroscope reset operation is enabled.
	 *
	 * This boolean variable is used to determine if the gyroscope should be reset
	 * to its default state. When set to true, the system will perform the
	 * necessary operations to reset and recalibrate the gyroscope. When set to
	 * false, the gyroscope continues operating without reset.
	 *
	 * @details The variable is typically utilized in applications or systems
	 * where gyroscope functionality is integrated, such as motion sensing
	 * for gaming controllers, virtual reality devices, or other motion-driven
	 * systems. Resetting the gyroscope might be necessary in scenarios where
	 * recalibration is required due to drift or unexpected behavior.
	 */
	bool bIsResetGyroscope = false;
	/**
	 * @brief Represents the context of a Human Interface Device (HID) used by
	 * DualSense controllers.
	 *
	 * This variable holds the FDeviceContext structure, which encapsulates the
	 * necessary state and configuration details for managing the connection,
	 * communication, and interaction with a specific DualSense gamepad device. It
	 * is primarily used within the UDualSenseLibrary class for initialization,
	 * input handling, and managing device-specific settings.
	 */
	FDeviceContext HIDDeviceContexts;
};
