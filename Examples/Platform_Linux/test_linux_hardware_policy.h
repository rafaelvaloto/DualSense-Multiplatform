// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#ifdef BUILD_GAMEPAD_CORE_TESTS
#include "GCore/Templates/TGenericHardwareInfo.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "test_linux_device_info.h"
#include <string>

// Sample Linux hardware policy adapter template
//
// This example satisfies the `IsHardwarePolicy` concept used by
// `GamepadCore::TGenericHardwareInfo`. Replace the bodies with calls to your
// concrete Linux implementation in
// (e.g., forward to your FCommonsDeviceInfo logic that uses SDL HID).
namespace Ftest_linux_platform
{
	struct Ftest_linux_hardware_policy;
	using Ftest_linux_hardware = GamepadCore::TGenericHardwareInfo<Ftest_linux_hardware_policy>;

	struct Ftest_linux_hardware_policy
	{
		Ftest_linux_hardware_policy() = default;

		void Read(FDeviceContext* Context)
		{
			Ftest_linux_device_info::Read(Context);
		}

		void Write(FDeviceContext* Context)
		{
			Ftest_linux_device_info::Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			Ftest_linux_device_info::Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			return Ftest_linux_device_info::CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			Ftest_linux_device_info::InvalidateHandle(Context);
		}

		void ProcessAudioHaptic(FDeviceContext* Context)
		{
			Ftest_linux_device_info::ProcessAudioHapitc(Context);
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
					// Verify it has 4 channels (stereo + haptic L/R)
					// DualSense audio device typically has 4 channels for haptics
					foundDeviceId = pPlaybackInfos[i].id;
					pFoundDeviceId = &foundDeviceId;
					break;
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
} // namespace Ftest_linux_platform
#endif
