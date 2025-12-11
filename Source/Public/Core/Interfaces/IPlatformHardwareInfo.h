// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#pragma once

#include "Core/Types/DSCoreTypes.h"
#include "Core/Types/Structs/Context/DeviceContext.h"

#define SONY_ (PLATFORM_PS4 || PLATFORM_PS5)

/**
 * Interface for platform-specific hardware information and operations.
 *
 * This class serves as a base interface for interacting with hardware devices
 * in a platform-agnostic manner. Derived classes must implement the provided
 * pure virtual methods to enable functionality specific to their platform or
 * device type.
 *
 * The design of the interface includes methods for creating and managing device
 * handles, reading and writing data, detecting devices, and handling cleanup
 * or invalidation. Additionally, it provides a mechanism for accessing a
 * singleton instance for the platform-specific implementation of the interface.
 */
class IPlatformHardwareInfo
{
	/**
	 * Provides access to the singleton instance of the IPlatformHardwareInfo.
	 *
	 * This method returns a reference to the singleton instance of the
	 * platform-specific hardware interface. If the instance does not exist, it is
	 * created depending on the platform.
	 *
	 * For example, on Windows platforms, the instance is initialized as
	 * `FHIDDeviceInfo`. For other platforms where no specific implementation
	 * exists, the instance is set to `nullptr`.
	 *
	 * @return A reference to the singleton platform hardware information
	 * interface.
	 *
	 * @note On platforms where the interface is not implemented, accessing this
	 * function may result in undefined behavior (e.g., dereferencing a null
	 * pointer).
	 */
public:
	static IPlatformHardwareInfo& Get();
	/**
	 * Sets the singleton instance of the platform-specific hardware information manager.
	 *
	 * This method allows assigning a custom implementation of the IPlatformHardwareInfo
	 * interface to act as the global instance for managing platform-specific hardware
	 * operations. It replaces any previously set instance with the provided one.
	 *
	 * @param Instance A unique pointer to the new implementation of IPlatformHardwareInfo
	 *                 that will serve as the global platform hardware manager instance.
	 *                 Ownership of the instance is transferred to the class.
	 */
	static void SetInstance(std::unique_ptr<IPlatformHardwareInfo> Instance);
	/**
	 * Virtual destructor for the IIPlatformHardwareInfo.
	 *
	 * This destructor ensures that derived classes are properly cleaned up when
	 * an object is deleted through a pointer to this interface. Being a virtual
	 * destructor, it guarantees that the destructor of the most derived class is
	 * invoked, preventing resource leaks or incomplete cleanup scenarios.
	 *
	 * @note The default implementation of the destructor is empty, as specific
	 *       cleanup should be handled in the derived implementations.
	 */
	virtual ~IPlatformHardwareInfo() = default;
	/**
	 * Reads data from the hardware device using the provided context.
	 *
	 * This pure virtual function must be implemented by derived classes to
	 * facilitate data reading from a specific hardware device. The implementation
	 * is responsible for interacting with the device and processing any relevant
	 * data.
	 *
	 * @param Context A pointer to the device context that provides the necessary
	 * information or state required to perform the read operation.
	 */
	virtual void Read(FDeviceContext* Context) = 0;
	/**
	 * Writes data to the hardware device using the provided context.
	 *
	 * This pure virtual function must be implemented by derived classes to enable
	 * data writing to a specific hardware device. Implementations should define
	 * the mechanism for sending data to the device according to its requirements.
	 *
	 * @param Context A pointer to the device context that provides the necessary
	 * information or state required to perform the write operation.
	 */
	virtual void Write(FDeviceContext* Context) = 0;
	/**
	 * Detects and collects information about connected hardware devices.
	 *
	 * This pure virtual method must be implemented by derived classes to scan for
	 * hardware devices and populate the provided array with their contexts. The
	 * implementation should detect all available devices and fill the array with
	 * corresponding device context information.
	 *
	 * @param Devices A reference to an array that will be populated with detected
	 * device contexts. Each entry represents a connected hardware device and its
	 * details.
	 */
	virtual void Detect(std::vector<FDeviceContext>& Devices) = 0;
	/**
	 * Initializes and creates a handle for the specified device context.
	 *
	 * This pure virtual method is intended to be implemented by derived classes
	 * to create a handle for a specific hardware device using the provided
	 * device context. The handle is typically used for subsequent operations
	 * (e.g., reading or writing data) with the associated hardware device.
	 *
	 * @param Context A pointer to the device context that provides details or
	 *                state required to create the handle for the hardware device.
	 * @return True if the handle is successfully created, false otherwise.
	 */
	virtual bool CreateHandle(FDeviceContext* Context) = 0;
	/**
	 * Invalidates the handle associated with the specified device context.
	 *
	 * This pure virtual method is intended to be implemented by derived classes
	 * to release or mark the handle associated with a hardware device as invalid.
	 * This operation is typically performed during cleanup or when the device is
	 * no longer in use to prevent further operations using an invalid or stale
	 * handle.
	 *
	 * @param Context A pointer to the device context associated with the handle
	 *                that needs to be invalidated. The context provides the
	 * relevant device information required for this operation.
	 */
	virtual void InvalidateHandle(FDeviceContext* Context) = 0;
	/**
	 * Processes audio haptic feedback for the given device context.
	 *
	 * Derived classes must implement this method to enable platform-specific
	 * handling of audio input and its transformation into haptic output. This is
	 * typically used for creating immersive feedback experiences where audio
	 * signals drive haptic responses.
	 *
	 * @param Context Pointer to the device context used to process audio haptic
	 * feedback.
	 */
	virtual void ProcessAudioHapitc(FDeviceContext* Context) = 0;
	/**
	 * Default constructor for the IIPlatformHardwareInfo.
	 *
	 * This protected constructor ensures that instances of the interface cannot
	 * be created directly. Derived classes can utilize this default constructor
	 * to initialize base functionality.
	 *
	 * @return An initialized instance of IPlatformHardwareInfoInterface.
	 */
protected:
	IPlatformHardwareInfo() = default;

	/**
	 * Initializes the singleton instance of the platform hardware information
	 * interface to nullptr.
	 *
	 * This static member is used to store the singleton instance of the
	 * IIPlatformHardwareInfo for the current platform. Initially, it is set to
	 * nullptr, and it will be instantiated with a platform-specific
	 * implementation when accessed via the appropriate getter method.
	 *
	 * @note The actual instance is platform-dependent and will be created as
	 * required during runtime.
	 */
private:
	static std::unique_ptr<IPlatformHardwareInfo> PlatformInfoInstance;
};
