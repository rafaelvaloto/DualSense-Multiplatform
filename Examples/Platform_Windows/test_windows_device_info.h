// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once
#ifdef BUILD_GAMEPAD_CORE_TESTS


#ifdef _WIN32
#include <Windows.h>

#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"

/**
 * @brief Enumerates the possible outcomes of a polling operation in HID device communication.
 *
 * This enum is used to represent the results of data polling when interacting with HID devices.
 * It provides status indicators for successful reads, lack of data in the current tick,
 * transient errors, or disconnected devices.
 */
enum class ETest_PollResult
{
	ReadOk,
	NoIoThisTick,
	TransientError,
	Disconnected
};

/**
 * @brief Represents a static class for HID device management and operations.
 *
 * This class encapsulates various tasks related to managing multiple HID devices, including handling connections,
 * transmitting and receiving data, detecting device presence, and managing device-specific states or contexts.
 */
class Ftest_windows_device_info
{

public:
	virtual ~Ftest_windows_device_info() = default;
	static void ProcessAudioHapitc(FDeviceContext* Context);
	/**
	 * @brief Configures Bluetooth-specific features for a given HID device.
	 *
	 * This method is used to initialize and set up Bluetooth-related features on a HID device.
	 * It communicates with the device using a feature report and updates the provided device context accordingly.
	 *
	 * @param Context A reference to the device context object that holds device-specific settings and state information.
	 * @return A boolean indicating whether the Bluetooth feature configuration was successful (true) or failed (false).
	 */
	static void ConfigureFeatures(FDeviceContext* Context);
	/**
	 * @brief Reads data from the specified HID device context.
	 *
	 * This method performs the process of retrieving input data from an HID device, verifying the context's validity,
	 * handling connection states, and managing input buffers. It also utilizes polling mechanisms to detect and
	 * report disconnection events or communication failures during the read process.
	 *
	 * @param Context Pointer to the device context representing the HID device being read. Must not be null and
	 *        should contain a valid device handle and input buffer. If the context is invalid or disconnected,
	 *        the method will handle associated cleanup and reporting.
	 */
	static void Read(FDeviceContext* Context);
	/**
	 * @brief Writes data to the specified HID device context.
	 *
	 * This method handles sending data to an HID device using the provided device context.
	 * It determines the appropriate output report size based on the device type and connection type,
	 * ensures the device handle is valid, and writes the data. Handles errors such as failed writes
	 * or invalid handles and logs necessary debugging information.
	 *
	 * @param Context Pointer to the device context containing relevant information such as the device
	 *        handle, connection type, device type, and output buffer. Must not be null and must
	 *        represent a valid device handle for a successful write operation.
	 */
	static void Write(FDeviceContext* Context);
	/**
	 * @brief Detects available HID devices and updates the provided list of device contexts.
	 *
	 * This method performs an enumeration of all connected HID devices that are currently present and
	 * accessible on the system. It retrieves their attributes, device paths, and initializes valid device
	 * contexts for further interaction. Devices that cannot be accessed or initialized are skipped.
	 *
	 * @param Devices A reference to an array of FDeviceContext objects that will be updated to include
	 *        the detected and initialized HID device contexts. Existing data in the array will be overwritten.
	 */
	static void Detect(std::vector<FDeviceContext>& Devices);
	/**
	 * @brief Creates a handle for the specified device context.
	 *
	 * This method attempts to open a communication channel with the device specified in the provided
	 * context by creating a handle. If successful, the handle is associated with the device context.
	 *
	 * @param Context A pointer to the device context containing the device path and other relevant information.
	 * @return True if the handle creation is successful; otherwise, false.
	 */
	static bool CreateHandle(FDeviceContext* Context);
	/**
	 * @brief Invalidates the handle of the specified HID device context and updates its connection status.
	 *
	 * This method ensures that the handle associated with the provided device context is properly invalidated.
	 * If the handle is valid, it will be closed and set to INVALID_HANDLE_VALUE. The connection status of the
	 * device context will also be updated to indicate that the device is no longer connected.
	 *
	 * @param Context Pointer to the device context representing the HID device whose handle is to be invalidated.
	 *        If the provided context is null, the method will return without performing any operations.
	 */
	static void InvalidateHandle(FDeviceContext* Context);
	/**
	 * @brief Invalidates the specified handle to prevent further use.
	 *
	 * This method ensures that the provided handle is marked invalid, preventing unintended operations on invalid resources.
	 *
	 * @param Handle The handle to be invalidated.
	 */
	static void InvalidateHandle(HANDLE Handle);
	/**
	 * @brief Sends a single ping operation to check the state of the specified handle.
	 *
	 * This method determines whether the provided handle is in a valid state and retrieves
	 * additional error information if the ping operation fails.
	 *
	 * @param Handle The handle to be checked.
	 * @param OutLastError Pointer to an integer that receives the error code if the operation fails.
	 *                     On successful execution, this will be set to ERROR_SUCCESS.
	 * @return True if the ping operation succeeds; otherwise, false.
	 */
	static bool PingOnce(HANDLE Handle, std::int32_t* OutLastError = nullptr);
	/**
	 * @brief Polls and processes a single tick for a HID device, performing ping and read operations.
	 *
	 * This method checks the device's state, performs a ping operation if necessary, and reads data from the device.
	 * It updates the polling state accordingly and determines the result of this polling iteration.
	 *
	 * @param Handle A handle to the HID device being polled.
	 * @param Buffer A pointer to a buffer where the method writes the data read from the device.
	 * @param Length The maximum number of bytes that can be read into the buffer.
	 * @param OutBytesRead A reference to a variable where the number of bytes successfully read will be stored.
	 * @return An enumeration value of type EPollResult indicating the result of the polling operation.
	 */
	static ETest_PollResult PollTick(HANDLE Handle, unsigned char* Buffer, std::int32_t Length, DWORD& OutBytesRead);
	/**
	 * @brief Determines whether the given error code should be treated as a device disconnection.
	 *
	 * This method evaluates the provided error code and identifies if it corresponds
	 * to a scenario where a device should be considered as disconnected.
	 *
	 * @param Error The error code to evaluate.
	 * @return true if the error code indicates a device disconnection, false otherwise.
	 */
	static bool ShouldTreatAsDisconnected(const std::int32_t Error)
	{
		switch (Error)
		{
			case ERROR_DEVICE_NOT_CONNECTED:
			case ERROR_GEN_FAILURE:
			case ERROR_INVALID_HANDLE:
			case ERROR_BAD_COMMAND:
			case ERROR_FILE_NOT_FOUND:
			case ERROR_ACCESS_DENIED: return true;
			default: return false;
		}
	}
};

#endif
#endif
