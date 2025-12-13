// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "WindowsDeviceInfo.h"
#include "Core/Templates/TGenericHardwareInfo.h"

namespace FWindowsPlatform
{
	struct FWindowsHardwarePolicy;
	using FWindowsHardware = GamepadCore::TGenericHardwareInfo<FWindowsHardwarePolicy>;
	
	struct FWindowsHardwarePolicy
	{
	public:
		void Read(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::Read(Context);
		}

		void Write(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			FWindowsDeviceInfo::Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			return FWindowsDeviceInfo::CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::InvalidateHandle(Context);
		}

		void ProcessAudioHaptic(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::ProcessAudioHapitc(Context);
		}
	};
}
