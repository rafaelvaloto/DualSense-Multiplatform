// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025

#define _FUNCTIONDISCOVERYKEYS_DEV_PKEY_H_
#include "test_windows_device_info.h"
#include <initguid.h>
#include <iostream>
#include <setupapi.h>
#include <windows.h>
#ifdef BUILD_GAMEPAD_CORE_TESTS

extern "C"
{
#include <hidsdi.h>
}

#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/Structs/Config/GamepadCalibration.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "GImplementations/Utils/GamepadSensors.h"
#include <filesystem>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <setupapi.h>
#include <vector>

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

void Ftest_windows_device_info::Detect(std::vector<FDeviceContext>& Devices)
{
	GUID HidGuid;
	HidD_GetHidGuid(&HidGuid);

	const HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(&HidGuid, nullptr, nullptr,
	                                                   DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return;
	}

	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {};
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	std::unordered_map<int, std::string> DevicePaths;
	for (int DeviceIndex = 0; SetupDiEnumDeviceInterfaces(DeviceInfoSet, nullptr, &HidGuid, DeviceIndex,
	                                                      &DeviceInterfaceData);
	     DeviceIndex++)
	{
		DWORD RequiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, nullptr, 0, &RequiredSize, nullptr);

		const auto DetailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(RequiredSize));
		if (!DetailDataBuffer)
		{
			continue;
		}

		DetailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, DetailDataBuffer, RequiredSize,
		                                    nullptr, nullptr))
		{
			const HANDLE TempDeviceHandle = CreateFileW(
			    DetailDataBuffer->DevicePath,
			    GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

			if (TempDeviceHandle != INVALID_HANDLE_VALUE)
			{
				HIDD_ATTRIBUTES Attributes = {};
				Attributes.Size = sizeof(HIDD_ATTRIBUTES);

				if (HidD_GetAttributes(TempDeviceHandle, &Attributes))
				{
					if (
					    Attributes.VendorID == 0x054C &&
					    (Attributes.ProductID == 0x0CE6 ||
					     Attributes.ProductID == 0x0DF2 ||
					     Attributes.ProductID == 0x05C4 ||
					     Attributes.ProductID == 0x09CC))
					{
						FDeviceContext Context = {};
						wchar_t DeviceProductString[260];
						if (HidD_GetProductString(TempDeviceHandle, DeviceProductString, 260))
						{
							if (DevicePaths.find(DeviceIndex) == DevicePaths.end())
							{
								std::string FinalString = std::filesystem::path(DetailDataBuffer->DevicePath).string();
								Context.Path = FinalString;
								DevicePaths[DeviceIndex] = FinalString;
								switch (Attributes.ProductID)
								{
									case 0x05C4:
									case 0x09CC:
										Context.DeviceType = EDSDeviceType::DualShock4;
										break;
									case 0x0DF2:
										Context.DeviceType = EDSDeviceType::DualSenseEdge;
										break;
									default: Context.DeviceType = EDSDeviceType::DualSense;
								}

								Context.IsConnected = true;
								Context.ConnectionType = EDSDeviceConnection::Usb;
								const std::string BtGuid = "{00001124-0000-1000-8000-00805f9b34fb}";
								if (FinalString.find(BtGuid) != std::string::npos ||
								    FinalString.find("bth") != std::string::npos ||
								    FinalString.find("BTHENUM") != std::string::npos)
								{
									Context.ConnectionType = EDSDeviceConnection::Bluetooth;
								}
							}
							Devices.push_back(Context);
						}
					}
				}
			}
			CloseHandle(TempDeviceHandle);
		}
		free(DetailDataBuffer);
	}
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
}

void Ftest_windows_device_info::Read(FDeviceContext* Context)
{
	if (!Context)
	{
		return;
	}

	if (Context->Handle == INVALID_PLATFORM_HANDLE)
	{
		return;
	}

	if (!Context->IsConnected)
	{
		return;
	}

	DWORD BytesRead = 0;
	if (Context->ConnectionType == EDSDeviceConnection::Bluetooth && Context->DeviceType == EDSDeviceType::DualShock4)
	{
		constexpr size_t InputReportLength = 547;
		PollTick(Context->Handle, Context->BufferDS4, InputReportLength, BytesRead);
	}
	else
	{
		const size_t InputBufferSize = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : 64;
		PollTick(Context->Handle, Context->Buffer, InputBufferSize, BytesRead);
	}
}

void Ftest_windows_device_info::Write(FDeviceContext* Context)
{
	if (Context->Handle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	size_t OutputReportLength = 32;
	if (Context->DeviceType == EDSDeviceType::DualShock4)
	{
		OutputReportLength = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : 32;
	}
	else
	{
		// DualSense
		OutputReportLength = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : 64;
	}

	DWORD BytesWritten = 0;
	if (!WriteFile(Context->Handle, Context->GetRawOutputBuffer(), (DWORD)OutputReportLength, &BytesWritten, nullptr))
	{
		std::cout << "Error writing to device " << GetLastError() << std::endl;
	}
}

bool Ftest_windows_device_info::CreateHandle(FDeviceContext* DeviceContext)
{
	std::string Source = DeviceContext->Path;
	std::wstring MyStdString = std::filesystem::path(Source).wstring();
	const HANDLE DeviceHandle = CreateFileW(
	    MyStdString.data(),
	    GENERIC_READ | GENERIC_WRITE,
	    FILE_SHARE_READ | FILE_SHARE_WRITE,
	    nullptr,
	    OPEN_EXISTING,
	    0,
	    nullptr);

	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		DeviceContext->Handle = INVALID_PLATFORM_HANDLE;
		return false;
	}

	HANDLE DuplicatedHandle = INVALID_HANDLE_VALUE;
	if (DuplicateHandle(GetCurrentProcess(), DeviceHandle, GetCurrentProcess(), &DuplicatedHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		CloseHandle(DeviceHandle);
		DeviceContext->Handle = DuplicatedHandle;
	}
	else
	{
		DeviceContext->Handle = DeviceHandle;
	}

	ConfigureFeatures(DeviceContext);
	return true;
}

void Ftest_windows_device_info::InvalidateHandle(FDeviceContext* Context)
{
	if (!Context)
	{
		return;
	}

	if (Context->Handle != INVALID_PLATFORM_HANDLE)
	{
		CloseHandle(Context->Handle);
		Context->Handle = INVALID_PLATFORM_HANDLE;
		Context->IsConnected = false;
		Context->Path.clear();

		std::memset(Context->Buffer, 0, sizeof(Context->Buffer));
		std::memset(Context->BufferDS4, 0, sizeof(Context->BufferDS4));
		std::memset(Context->BufferAudio, 0, sizeof(Context->BufferAudio));

		unsigned char* RawOutput = Context->GetRawOutputBuffer();
		std::memset(RawOutput, 0, 78);
	}
}

void Ftest_windows_device_info::InvalidateHandle(HANDLE Handle)
{
	if (Handle != INVALID_PLATFORM_HANDLE)
	{
		CloseHandle(Handle);
	}
}

ETest_PollResult Ftest_windows_device_info::PollTick(HANDLE Handle, unsigned char* Buffer, std::int32_t Length, DWORD& OutBytesRead)
{
	std::int32_t Err = ERROR_SUCCESS;
	PingOnce(Handle, &Err);

	OutBytesRead = 0;
	if (!ReadFile(Handle, Buffer, Length, &OutBytesRead, nullptr))
	{
		return ETest_PollResult::Disconnected;
	}

	return ETest_PollResult::ReadOk;
}

bool Ftest_windows_device_info::PingOnce(HANDLE Handle, std::int32_t* OutLastError)
{
	FILE_STANDARD_INFO Info{};
	if (!GetFileInformationByHandleEx(Handle, FileStandardInfo, &Info, sizeof(Info)))
	{
		if (OutLastError)
		{
			*OutLastError = GetLastError();
		}
		return false;
	}
	if (OutLastError)
	{
		*OutLastError = ERROR_SUCCESS;
	}
	return true;
}

std::string Ftest_windows_device_info::GetContainerId(const std::string& DevicePath)
{
	std::wstring WPath(DevicePath.begin(), DevicePath.end());
	GUID HidGuid;
	HidD_GetHidGuid(&HidGuid);

	HDEVINFO DeviceInfoSet = SetupDiGetClassDevsW(&HidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return "";
	}

	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {sizeof(SP_DEVICE_INTERFACE_DATA)};
	if (SetupDiOpenDeviceInterfaceW(DeviceInfoSet, WPath.c_str(), 0, &DeviceInterfaceData))
	{
		SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};
		// DetailData is needed to get the DevInfoData associated with the interface
		DWORD RequiredSize = 0;
		SetupDiGetDeviceInterfaceDetailW(DeviceInfoSet, &DeviceInterfaceData, nullptr, 0, &RequiredSize, nullptr);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			std::vector<char> Buffer(RequiredSize);
			PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)Buffer.data();
			pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
			if (SetupDiGetDeviceInterfaceDetailW(DeviceInfoSet, &DeviceInterfaceData, pDetail, RequiredSize, nullptr, &DeviceInfoData))
			{
				DEVPROPTYPE PropType;
				GUID ContainerId = {0};
				if (SetupDiGetDevicePropertyW(DeviceInfoSet, &DeviceInfoData, &DEVPKEY_Device_ContainerId, &PropType, (PBYTE)&ContainerId, sizeof(GUID), nullptr, 0))
				{
					wchar_t GuidString[40];
					StringFromGUID2(ContainerId, GuidString, 40);
					SetupDiDestroyDeviceInfoList(DeviceInfoSet);

					char GuidStr[40];
					WideCharToMultiByte(CP_ACP, 0, GuidString, -1, GuidStr, 40, nullptr, nullptr);
					return std::string(GuidStr);
				}
			}
		}
	}
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	return "";
}

std::string Ftest_windows_device_info::GetAudioContainerId(const wchar_t* AudioDeviceId)
{
	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDevice* pDevice = nullptr;
	IPropertyStore* pProps = nullptr;
	std::string Result = "";

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
	if (SUCCEEDED(hr))
	{
		hr = pEnumerator->GetDevice(AudioDeviceId, &pDevice);
		if (SUCCEEDED(hr))
		{
			hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT var;
				PropVariantInit(&var);
				hr = pProps->GetValue(PKEY_Device_ContainerId, &var);
				if (SUCCEEDED(hr) && var.vt == VT_CLSID)
				{
					wchar_t GuidString[40];
					StringFromGUID2(*var.puuid, GuidString, 40);

					char GuidStr[40];
					WideCharToMultiByte(CP_ACP, 0, GuidString, -1, GuidStr, 40, nullptr, nullptr);
					Result = std::string(GuidStr);
				}
				PropVariantClear(&var);
				pProps->Release();
			}
			pDevice->Release();
		}
		pEnumerator->Release();
	}
	return Result;
}

void Ftest_windows_device_info::ProcessAudioHaptic(FDeviceContext* Context)
{
	if (!Context || !Context->Handle)
	{
		return;
	}

	if (Context->ConnectionType != EDSDeviceConnection::Bluetooth)
	{
		return;
	}

	if (Context->Handle == INVALID_PLATFORM_HANDLE)
	{
		return;
	}

	unsigned long BytesWritten = 0;
	constexpr size_t BufferSize = 142;
	if (!WriteFile(Context->Handle, Context->BufferAudio, BufferSize, &BytesWritten, nullptr))
	{
		const unsigned long Error = GetLastError();
		if (Error != ERROR_IO_PENDING)
		{
		}
	}
}

void Ftest_windows_device_info::ConfigureFeatures(FDeviceContext* Context)
{
	using namespace FGamepadSensors;
	FGamepadCalibration Calibration;

	if (Context->DeviceType == EDSDeviceType::DualShock4)
	{
		if (Context->ConnectionType == EDSDeviceConnection::Usb)
		{
			unsigned char FeatureBuffer[37] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x02;
			if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 37))
			{
				const unsigned long Error = GetLastError();
				return;
			}

			DualShockCalibrationSensors(FeatureBuffer, Calibration, Context->ConnectionType);
		}
		else
		{

			unsigned char FeatureBuffer[41] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x05;
			if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 41))
			{
				const unsigned long Error = GetLastError();
				return;
			}

			DualShockCalibrationSensors(FeatureBuffer, Calibration, Context->ConnectionType);
		}

		Context->Calibration = Calibration;
	}
	else
	{
		unsigned char FeatureBuffer[41] = {0};
		std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

		FeatureBuffer[0] = 0x05;
		if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 41))
		{
			const unsigned long Error = GetLastError();
			return;
		}

		DualSenseCalibrationSensors(FeatureBuffer, Calibration);
		Context->Calibration = Calibration;
	}
}
#endif
