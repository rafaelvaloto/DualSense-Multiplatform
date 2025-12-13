// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once

#include "Async/TaskGraphInterfaces.h"
#include "Core/Interfaces/ISonyGamepad.h"
#include "Core/Templates/TBasicDeviceRegistry.h"
#include "CoreMinimal.h"
#include "DeviceRegistryPolicy.h"
#include "HAL/PlatformProcess.h"

/**
 * A manager class that handles the creation, storage, and lifecycle management of device library
 * instances associated with Sony gamepad controllers. This class ensures proper initialization,
 * cleanup, and access control for the controller devices.
 */
class FDeviceRegistry : public FNoncopyable
{
	
public:
	virtual ~FDeviceRegistry();
	/**
	 * Initializes the singleton instance of FDeviceRegistry.
	 * This method ensures that the FDeviceRegistry instance is created only once
	 * and is accessible for managing device-related operations. If the instance does not
	 * exist, it allocates and initializes a new instance.
	 */
	static void Initialize();
	/**
	 * Cleans up and releases resources associated with the device registry.
	 * This method ensures that all managed instances are properly reset,
	 * preventing resource leaks and ensuring a clean shutdown process.
	 */
	static void Shutdown();
	/**
	 * Retrieves the static instance of the FDeviceRegistry class. This method
	 * ensures that only a single instance of the manager class is created and provides
	 * global access to it for managing device library instances of Sony gamepad controllers.
	 *
	 * @return A pointer to the singleton instance of UDeviceContainerManager. Returns nullptr
	 *         if the instance has not been initialized.
	 */
	static FDeviceRegistry* Get();
	/**
	 * Discovers and registers connected devices in the device registry.
	 *
	 * This method performs a scan for all connected Sony gamepad controllers and
	 * updates the internal registry with the discovered devices. It ensures that
	 * all relevant devices are recognized and properly managed by the registry.
	 * This function is typically used during initialization or when new devices
	 * are connected to ensure they are accounted for.
	 */
	static void DiscoverDevices(float DeltaTime);
	/**
	 * Retrieves the library instance associated with a specific input device ID.
	 *
	 * This function queries the internal registry implementation to obtain a valid instance
	 * of an ISonyGamepad associated with the given FInputDeviceId. If no such instance exists
	 * or the registry implementation is uninitialized, it returns nullptr.
	 *
	 * @param DeviceId The unique identifier of the input device for which the library instance is requested.
	 * @return A pointer to the ISonyGamepad instance associated with the specified input device,
	 *         or nullptr if none exists or the registry is not initialized.
	 */
	static ISonyGamepad* GetLibraryInstance(FInputDeviceId DeviceId);
	
	using FRegistryLogic = GamepadCore::TBasicDeviceRegistry<FDeviceRegistryPolicy>;
private:
	FDeviceRegistry();
	/**
	 * A static instance of the FDeviceRegistry, serving as the singleton instance
	 * for managing device library instances of Sony gamepad controllers. This variable
	 * ensures global access and consistent management of the controller devices, while
	 * enforcing a single unique instance of the manager class.
	 */
	static TUniquePtr<FDeviceRegistry> Instance;
	/**
	 * An internal implementation detail of the device registry system responsible for managing
	 * the underlying logic and operation of the registry. Encapsulates functionality for
	 * creating, storing, and retrieving device library instances for Sony gamepad controllers.
	 * Ensures clean separation and abstraction of the registry's internal mechanics.
	 */
	static std::unique_ptr<FRegistryLogic> RegistryImplementation;
	
};
