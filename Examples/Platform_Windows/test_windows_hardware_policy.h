// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#ifdef BUILD_GAMEPAD_CORE_TESTS
#include "GCore/Templates/TGenericHardwareInfo.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "test_windows_device_info.h"
#include <cwchar>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <setupapi.h>
#include <string>

#ifdef DEFINE_PROPERTYKEY
#undef DEFINE_PROPERTYKEY
#endif
#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
	extern "C" const __declspec(selectany) PROPERTYKEY name = {{l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}, pid}

#ifdef DEFINE_DEVPROPKEY
#undef DEFINE_DEVPROPKEY
#endif
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
	extern "C" const __declspec(selectany) DEVPROPKEY name = {{l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}, pid}

#include <initguid.h>

#ifdef PKEY_Device_ContainerId
#undef PKEY_Device_ContainerId
#endif
DEFINE_PROPERTYKEY(PKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2);

#ifndef DEVPKEY_Device_ContainerId
DEFINE_DEVPROPKEY(DEVPKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2);
#endif

#ifndef PKEY_NAME
DEFINE_PROPERTYKEY(PKEY_NAME, 0xb725f130, 0x47ef, 0x101a, 0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac, 10);
#endif

#define _FUNCTIONDISCOVERYKEYS_DEV_PKEY_H_

#pragma comment(lib, "Propsys.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Hid.lib")

#include <mutex>
#include <set>

namespace Ftest_windows_platform
{
	struct Ftest_windows_hardware_policy;
	using Ftest_windows_hardware = GamepadCore::TGenericHardwareInfo<Ftest_windows_hardware_policy>;

	/**
	 * @brief Global registry to track which audio devices are currently in use
	 * to ensure multiple DualSense controllers get separate audio channels.
	 */
	struct FAudioDeviceRegistry
	{
		static FAudioDeviceRegistry& Get()
		{
			static FAudioDeviceRegistry Instance;
			return Instance;
		}

		void RegisterDevice(const ma_device_id& DeviceId)
		{
			std::lock_guard<std::mutex> Lock(Mutex);
			UsedDeviceIds.insert(DeviceId);
		}

		void UnregisterDevice(const ma_device_id& DeviceId)
		{
			std::lock_guard<std::mutex> Lock(Mutex);
			UsedDeviceIds.erase(DeviceId);
		}

		bool IsDeviceInUse(const ma_device_id& DeviceId)
		{
			std::lock_guard<std::mutex> Lock(Mutex);
			return UsedDeviceIds.find(DeviceId) != UsedDeviceIds.end();
		}

	private:
		std::mutex Mutex;
		struct DeviceIdCompare
		{
			bool operator()(const ma_device_id& lhs, const ma_device_id& rhs) const
			{
				return std::memcmp(&lhs, &rhs, sizeof(ma_device_id)) < 0;
			}
		};
		std::set<ma_device_id, DeviceIdCompare> UsedDeviceIds;
	};

	struct Ftest_windows_hardware_policy
	{
	public:
		void Read(FDeviceContext* Context)
		{
			Ftest_windows_device_info::Read(Context);
		}

		void Write(FDeviceContext* Context)
		{
			Ftest_windows_device_info::Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			Ftest_windows_device_info::Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			return Ftest_windows_device_info::CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			if (Context && Context->AudioContext)
			{
				if (Context->AudioContext->bHasDeviceId)
				{
					FAudioDeviceRegistry::Get().UnregisterDevice(Context->AudioContext->DeviceId);
				}
			}
			Ftest_windows_device_info::InvalidateHandle(Context);
		}

		void ProcessAudioHaptic(FDeviceContext* Context)
		{
			Ftest_windows_device_info::ProcessAudioHaptic(Context);
		}

		/**
		 * @brief Initializes the audio device for a DualSense controller.
		 *
		 * Enumerates available audio playback devices, searches for one matching
		 * the DualSense controller (by name containing "DualSense" or "Wireless Controller"),
		 * and initializes the FAudioDeviceContext with the found device.
		 *
		 * @param Context The device context to store the audio device in
		 */
		void InitializeAudioDevice(FDeviceContext* Context)
		{
			if (!Context)
			{
				return;
			}

			// Initialize miniaudio context for device enumeration
			ma_context maContext;
			if (ma_context_init(nullptr, 0, nullptr, &maContext) != MA_SUCCESS)
			{
				return;
			}

			// Get playback devices
			ma_device_info* pPlaybackInfos;
			ma_uint32 playbackCount;
			ma_device_info* pCaptureInfos;
			ma_uint32 captureCount;

			if (ma_context_get_devices(&maContext, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS)
			{
				ma_context_uninit(&maContext);
				return;
			}

			// Get Gamepad Container ID
			std::string GamepadContainerId = Ftest_windows_device_info::GetContainerId(Context->Path);

			// Search for DualSense audio device
			ma_device_id* pFoundDeviceId = nullptr;
			ma_device_id foundDeviceId;

			for (ma_uint32 i = 0; i < playbackCount; i++)
			{
				std::string deviceName(pPlaybackInfos[i].name);

				// Check if device name contains DualSense identifiers
				// DualSense appears as "Wireless Controller" or "DualSense Wireless Controller"
				if (deviceName.find("DualSense") != std::string::npos ||
				    deviceName.find("Wireless Controller") != std::string::npos)
				{
					// Match by Container ID if possible
					if (!GamepadContainerId.empty())
					{
						std::string AudioContainerId = Ftest_windows_device_info::GetAudioContainerId(pPlaybackInfos[i].id.wasapi);
						if (AudioContainerId != GamepadContainerId)
						{
							continue;
						}
					}

					// Check if this device is already in use by another controller
					if (!FAudioDeviceRegistry::Get().IsDeviceInUse(pPlaybackInfos[i].id))
					{
						// Verify it has 4 channels (stereo + haptic L/R)
						// DualSense audio device typically has 4 channels for haptics
						foundDeviceId = pPlaybackInfos[i].id;
						pFoundDeviceId = &foundDeviceId;

						// Mark as used
						FAudioDeviceRegistry::Get().RegisterDevice(foundDeviceId);
						break;
					}
				}
			}

			// Initialize audio context with found device (or default if not found)
			Context->AudioContext = std::make_shared<FAudioDeviceContext>();

			if (pFoundDeviceId)
			{
				// Initialize with specific DualSense device
				// DualSense haptics use 4 channels at 48000 Hz
				Context->AudioContext->InitializeWithDeviceId(pFoundDeviceId, 48000, 4);
			}

			ma_context_uninit(&maContext);
		}
	};
} // namespace Ftest_windows_platform
#endif
