
#pragma once

#ifdef __unix__
#include "SDL.h" // SDL or hid
#include "Implementations/Utils/GamepadSensors.h"
#include <cstring>
#include <string>
#include <unordered_set>

namespace GamepadCore::Adapters::Platforms::Linux {
	struct FLinuxDeviceInfo {
		void Read(FDeviceContext* Context)
		{
			if (!Context || !Context->Handle)
			{
				return;
			}
			SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);
			if (!DeviceHandle)
			{
				return;
			}

			if (Context->ConnectionType == EDSDeviceConnection::Bluetooth &&
			    Context->DeviceType == EDSDeviceType::DualShock4)
			{
				const size_t InputReportLength = 547;
				if (SDL_hid_read(DeviceHandle, Context->BufferDS4, InputReportLength) < 0)
				{
					InvalidateHandle(Context);
				}
				return;
			}

			const size_t InputReportLength =
			    (Context->ConnectionType == EDSDeviceConnection::Bluetooth) ? 78 : 64;
			if (sizeof(Context->Buffer) < InputReportLength)
			{
				InvalidateHandle(Context);
				return;
			}

			if (SDL_hid_read(DeviceHandle, Context->Buffer, InputReportLength) < 0)
			{
				InvalidateHandle(Context);
			}
		}

		void ProcessAudioHapitc(FDeviceContext* Context)
		{
			if (!Context || !Context->Handle)
			{
				return;
			}

			SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);

			constexpr size_t Report = 142;
			int BytesWritten = SDL_hid_write(DeviceHandle, Context->BufferAudio, Report);
			if (BytesWritten < 0)
			{
			}
		}

		bool ConfigureFeatures(FDeviceContext* Context)
		{
			SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);

			unsigned char FeatureBuffer[41] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x05;
			if (!SDL_hid_get_feature_report(DeviceHandle, FeatureBuffer, 41))
			{
				return false;
			}

			using namespace FGamepadSensors;
			FGamepadCalibration Calibration;
			DualSenseCalibrationSensors(FeatureBuffer, Calibration);

			Context->Calibration = Calibration;
			return true;
		}

		void Write(FDeviceContext* Context)
		{
			if (!Context || !Context->Handle)
			{
				return;
			}

			SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);

			const size_t InReportLength =
			    (Context->DeviceType == EDSDeviceType::DualShock4) ? 32 : 74;
			const size_t OutputReportLength =
			    (Context->ConnectionType == EDSDeviceConnection::Bluetooth)
			        ? 78
			        : InReportLength;

			int BytesWritten =
			    SDL_hid_write(DeviceHandle, Context->BufferOutput, OutputReportLength);
			if (BytesWritten < 0)
			{
				InvalidateHandle(Context);
			}
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			Devices.clear();

			const std::unordered_set<uint16_t> SupportedPIDs = {
			    DUALSHOCK4_PID_V1, DUALSHOCK4_PID_V2, DUALSENSE_PID, DUALSENSE_EDGE_PID};

			SDL_hid_device_info* Devs = SDL_hid_enumerate(SONY_VENDOR_ID, 0);
			if (!Devs)
			{
				return;
			}

			for (SDL_hid_device_info* CurrentDevice = Devs; CurrentDevice != nullptr;
			     CurrentDevice = CurrentDevice->next)
			{
				if (SupportedPIDs.contains(CurrentDevice->product_id))
				{
					FDeviceContext NewDeviceContext;
					NewDeviceContext.Path = std::string(CurrentDevice->path);

					switch (CurrentDevice->product_id)
					{
						case DUALSHOCK4_PID_V1:
						case DUALSHOCK4_PID_V2:
							NewDeviceContext.DeviceType = EDSDeviceType::DualShock4;
							break;
						case DUALSENSE_EDGE_PID:
							NewDeviceContext.DeviceType = EDSDeviceType::DualSenseEdge;
							break;
						case DUALSENSE_PID:
						default:
							NewDeviceContext.DeviceType = EDSDeviceType::DualSense;
							break;
					}

					NewDeviceContext.IsConnected = true;
					if (CurrentDevice->interface_number == -1)
					{
						NewDeviceContext.ConnectionType = EDSDeviceConnection::Bluetooth;
					}
					else
					{
						NewDeviceContext.ConnectionType = EDSDeviceConnection::Usb;
					}
					NewDeviceContext.Handle = nullptr;
					Devices.push_back(NewDeviceContext);
				}
			}
			SDL_hid_free_enumeration(Devs);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			if (!Context)
			{
				return false;
			}

			const char* Path = Context->Path.data();
			const FPlatformDeviceHandle Handle = SDL_hid_open_path(Path, true);
			if (Handle == INVALID_PLATFORM_HANDLE)
			{
				return false;
			}

			SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Handle);
			SDL_hid_set_nonblocking(DeviceHandle, 1);
			Context->Handle = Handle;

			ConfigureFeatures(Context);
			return true;
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			if (Context)
			{
				SDL_hid_device* DeviceHandle =
				    static_cast<SDL_hid_device*>(Context->Handle);
				if (DeviceHandle != nullptr)
				{
					SDL_hid_close(DeviceHandle);
				}

				Context->Handle = INVALID_PLATFORM_HANDLE;
				Context->IsConnected = false;

				Context->Path.clear();
				std::memset(Context->Buffer, 0, sizeof(Context->Buffer));
				std::memset(Context->BufferDS4, 0, sizeof(Context->BufferDS4));
				std::memset(Context->BufferOutput, 0, sizeof(Context->BufferOutput));
				std::memset(Context->BufferAudio, 0, sizeof(Context->BufferAudio));
			}
		}
	};
}

#endif
