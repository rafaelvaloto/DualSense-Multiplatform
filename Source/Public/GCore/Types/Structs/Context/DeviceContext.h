// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "../../DSCoreTypes.h"
#include "../../ECoreGamepad.h"
#include "../Config/GamepadCalibration.h"
#include "AudioContext.h"
#include "InputContext.h"
#include "OutputContext.h"
#include "GCore/Utils/SoDefines.h"

using FPlatformDeviceHandle = void*;
#define INVALID_PLATFORM_HANDLE nullptr

/**
 * @brief Represents the context and state of a connected device.
 *
 * This class is primarily used for managing the connection, data buffer, and
 * attributes of devices in the DualSense HID management system. Each device
 * context contains handles, connection details, and configuration needed for
 * proper functionality.
 *
 * It is a crucial component for detecting, initializing, and managing devices
 * using related library systems.
 */
struct FDeviceContext
{
	/**
	 * @brief Raw device handle used for communication with a specific
	 * input/output hardware device.
	 *
	 * This handle represents a connection to the lower-level system or hardware
	 * interface and is primarily utilized for managing input/output operations,
	 * state retrieval, or other device-related activities.
	 *
	 * - The handle is created through platform-specific mechanisms, such as the
	 * `CreateHandle` function in `UDualSenseHIDManager`.
	 * - It may represent a connection to a physical hardware device or similar
	 * system resources.
	 *
	 * @note Handle validity and state should always be verified before usage, as
	 * invalid or disconnected handles can result in undefined behavior. For
	 * instance, it may hold `INVALID_HANDLE_VALUE` when invalid or disconnected.
	 */
	FPlatformDeviceHandle Handle{};
	/**
	 * @brief Manages the state and operations related to an audio device.
	 *
	 * This shared pointer points to an object that encapsulates the context
	 * and configuration of an audio device. It includes necessary resources
	 * for handling audio streams and ensures controlled access and proper
	 * lifecycle management of the associated audio device context.
	 *
	 * Used for facilitating audio-related functionality in the system.
	 */
	std::shared_ptr<FAudioDeviceContext> AudioContext;
	/**
	 * @brief Represents a file or resource path in the context of device
	 * management.
	 *
	 * This variable is used to store the location or identifier of a specific
	 * resource, configuration file, or executable path associated with the device
	 * or system operation.
	 *
	 * It plays a critical role in ensuring proper file system referencing and
	 * resource allocation within the management system.
	 */
	std::string Path;
	/**
	 * @brief Internal data buffer for device communication.
	 *
	 * This buffer serves as a temporary storage space for handling input and
	 * output data during device interactions. It is specifically used in
	 * processing data exchange within the DualSense HID management framework.
	 *
	 * The size of the buffer is designed to accommodate the expected data payload
	 * during these communications, ensuring efficient handling of device
	 * protocols.
	 */
	unsigned char Buffer[78] = {};
	/**
	 * @brief Internal data buffer for DualShock 4 Bluetooth communication.
	 *
	 * This buffer is specifically allocated for handling input and output data
	 * when communicating with DualShock 4 controllers connected via Bluetooth.
	 * It is used in the context of parsing and building device reports during
	 * Bluetooth transfers, ensuring compliance with the expected report format
	 * and size required by the DualShock 4 HID protocol.
	 *
	 * The buffer size of 547 bytes matches the standard payload requirements
	 * for DualShock 4 devices in Bluetooth communication scenarios.
	 *
	 * @note This buffer is not intended for DualSense controllers or USB
	 * connections; for those, use Buffer[78] instead.
	 */
	unsigned char BufferDS4[547] = {};
	/**
	 * @brief Stores the haptic audio data for DualSense devices.
	 *
	 * This buffer is used to handle and transfer haptic feedback audio data
	 * to a connected DualSense device, enabling advanced vibration and
	 * feedback mechanisms driven by audio signals.
	 */
	unsigned char BufferAudio[142] = {};
	/**
	 * @brief Holds calibration data for a gamepad device.
	 *
	 * This structure is utilized to store and manage calibration parameters
	 * such as offset, sensitivity, and range for gamepad inputs. The calibration
	 * ensures accurate and responsive control by compensating for hardware
	 * variances.
	 *
	 * Typically used during the initialization and configuration phase of a
	 * gamepad device to align input response with expected behavior.
	 */
	FGamepadCalibration Calibration;
	/**
	 * Indicates whether the device is connected.
	 *
	 * This boolean variable represents the connection state of a device.
	 * - `true` indicates the device is currently connected.
	 * - `false` indicates the device is not connected.
	 *
	 * It is used to manage and track the status of devices in the context
	 * of operations such as detection, initialization, and communication.
	 */
	bool IsConnected = false;
	/**
	 * Represents the output configuration for a device context, typically used
	 * to control advanced features of a connected DualSense controller.
	 *
	 * The FOutput structure manages outputs such as lightbar color, microphone
	 * light states, player indicator LEDs, rumble motor intensities, and haptic
	 * trigger effects. Additionally, it includes audio and general feature
	 * configurations.
	 *
	 * This variable is initialized within a device context and used in
	 * conjunction with runtime operations to apply desired settings to the
	 * connected controller.
	 */
	FOutputContext Output;
	/**
	 * Specifies the type of connection used by a device.
	 *
	 * ConnectionType is an instance of the EDeviceConnection enumeration,
	 * which defines the connection modes a device can use. Supported connection
	 * types include USB, Bluetooth, and Unrecognized.
	 *
	 * This variable is primarily used in the context of managing device
	 * communication, where it helps determine the appropriate handling for input
	 * and output operations.
	 *
	 * Possible values:
	 * - Usb: Represents devices connected via USB.
	 * - Bluetooth: Represents devices connected via Bluetooth.
	 * - Unrecognized: Represents devices with an unknown or unsupported
	 * connection type.
	 *
	 * In scenarios such as device discovery or IO operations, ConnectionType
	 * influences logic such as input report length, output buffering, and
	 * connection state updates.
	 */
	EDSDeviceConnection ConnectionType = EDSDeviceConnection::Unrecognized;
	/**
	 * @brief Represents the type of device in the context of DualSense HID
	 * management.
	 *
	 * DeviceType specifies the category or model of the connected device,
	 * enabling the system to adjust its behavior or configurations accordingly.
	 * Examples include default DualSense controllers, the Edge variation, and
	 * older DualShock models.
	 *
	 * It plays a pivotal role in distinguishing devices for operations such as
	 * feature initialization, compatibility checks, and tailored input/output
	 * processing.
	 */
	EDSDeviceType DeviceType = EDSDeviceType::NotFound;

	// Runtime override for trigger bytes [10..20] (Right) and [21..31] (Left) in
	// the DualSense output buffer. When enabled via console commands, these
	// arrays are copied verbatim into the HID report.
	bool bOverrideTriggerBytes = false;
	unsigned char OverrideTriggerRight[10] = {};
	unsigned char OverrideTriggerLeft[10] = {};

	bool bEnableTouch = false;
	bool bEnableGesture = false;
	bool bIsResetGyroscope = false;
	bool bEnableAccelerometerAndGyroscope = false;
	/**
	 * A map representing the states of various buttons on a controller.
	 *
	 * Each key in the map is a button name (std::string), and its associated
	 * value is a boolean indicating whether the button is currently pressed
	 * (`true`) or not pressed (`false`).
	 *
	 * This variable is primarily used for tracking button input states and
	 * ensuring accurate representation of input events, such as detecting when a
	 * button's state changes from pressed to released or vice versa.
	 *
	 * The map is updated dynamically during controller runtime, including in
	 * functions like CheckButtonInput, which ensures real-time synchronization of
	 * input states. It is reset during library shutdown to clear all stored
	 * button states.
	 */
	std::unordered_map<std::string, bool> ButtonStates;
	/**
	 * @typedef AnalogStates
	 * @brief Represents a mapping of analog input states in the DualSense
	 * library.
	 *
	 * AnalogStates is a container that maps unique input identifiers, represented
	 * by FName, to their respective float values, which typically denote the
	 * state or intensity of analog inputs.
	 *
	 * This map is used to handle and store the state of analog inputs, such as
	 * triggers or sticks, providing a mechanism to track their values for input
	 * handling or processing purposes in an application.
	 *
	 * @details The keys in this map (std::string) are designed to uniquely
	 * identify different analog input sources, while the associated float values
	 * represent their corresponding state, usually on a normalized scale.
	 */
	std::unordered_map<std::string, float> AnalogStates;

	/**
	 * @brief Ensures thread-safe access to shared input resources.
	 *
	 * This mutex is used to synchronize access to input-related data or
	 * operations, preventing race conditions in multithreaded environments. It is
	 * essential for maintaining data consistency and avoiding concurrent
	 * modification issues.
	 */
 mutable gc_lock::mutex InputMutex;
 mutable gc_lock::mutex OutputMutex;

	unsigned char* GetRawOutputBuffer() { return BufferOutput; }

protected:
	/**
	 * @brief Handles input-specific operations and data for connected devices.
	 *
	 * This structure is responsible for managing input-related contexts such as
	 * capturing, processing, and routing input data from connected devices. It
	 * plays a central role in the integration of user-device interactions within
	 * the system.
	 */
	FInputContext Input;
	/**
	 * @brief Represents the input processing context running on the game thread.
	 *
	 * This variable is utilized to manage and coordinate input-related operations
	 * specifically executed on the game thread. It ensures that input handling
	 * remains synchronized with the game loop, providing a stable and consistent
	 * flow for processing player inputs and system events.
	 *
	 * It is a critical component for maintaining thread safety and proper
	 * execution of input logic within the game engine.
	 */
	FInputContext InputGameThread;

private:
	/**
	 * A fixed-size buffer for storing input or output data associated with a
	 * device context. This buffer is utilized for reading device input reports or
	 * for other data management tasks. Its size is defined as 78 bytes to
	 * accommodate device input requirements, particularly for devices connected
	 * over Bluetooth.
	 *
	 * Buffer is a critical component of FDeviceContext and is managed to ensure
	 * data consistency and proper memory zeroing upon disconnection or failure.
	 *
	 * @note The size of this buffer is determined by the specific device
	 * connection type or input requirements, ensuring compatibility and
	 * sufficient data handling capabilities.
	 */
	unsigned char BufferOutput[78] = {};

public:
	FDeviceContext() = default;

	FDeviceContext(const FDeviceContext& Other) { *this = Other; }

	FDeviceContext& operator=(const FDeviceContext& Other)
	{
		if (this != &Other)
		{
			Handle = Other.Handle;
			Path = Other.Path;

			std::memcpy(Buffer, Other.Buffer, sizeof(Buffer));
			std::memcpy(BufferDS4, Other.BufferDS4, sizeof(BufferDS4));
			std::memcpy(BufferAudio, Other.BufferAudio, sizeof(BufferAudio));
			std::memcpy(BufferOutput, Other.BufferOutput, sizeof(BufferOutput));

			// Auxiliary state variables
			bEnableTouch = Other.bEnableTouch;
			bEnableGesture = Other.bEnableGesture;
			bIsResetGyroscope = Other.bIsResetGyroscope;
			bEnableAccelerometerAndGyroscope = Other.bEnableAccelerometerAndGyroscope;

			// Auxiliary status variables
			Calibration = Other.Calibration;
			IsConnected = Other.IsConnected;
			DeviceType = Other.DeviceType;
			ConnectionType = Other.ConnectionType;

			bOverrideTriggerBytes = Other.bOverrideTriggerBytes;
			std::memcpy(OverrideTriggerRight, Other.OverrideTriggerRight,
			            sizeof(OverrideTriggerRight));
			std::memcpy(OverrideTriggerLeft, Other.OverrideTriggerLeft,
			            sizeof(OverrideTriggerLeft));

			ButtonStates = Other.ButtonStates;
			AnalogStates = Other.AnalogStates;

			Output = Other.Output;
			Input = Other.Input;
			InputGameThread = Other.InputGameThread;
		}
		return *this;
	}

	FInputContext* GetInputState()
	{
		gc_lock::lock_guard<gc_lock::mutex> Lock(InputMutex);
		return &InputGameThread;
	}

	// [BackgroundThread]
	FInputContext* GetBackBuffer() { return &Input; }

	// [BackgroundThread] change buffers
	void SwapInputBuffers()
	{
		gc_lock::lock_guard<gc_lock::mutex> Lock(InputMutex);
		InputGameThread = Input;
	}
};
