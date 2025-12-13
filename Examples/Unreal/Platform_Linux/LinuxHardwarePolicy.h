// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Templates/TGenericHardwareInfo.h"
#include "Core/Types/Structs/Context/DeviceContext.h"
#include "Implementations/Platforms/Commons/CommonsDeviceInfo.h"


// Sample Linux hardware policy adapter template
//
// This example satisfies the `IsHardwarePolicy` concept used by
// `GamepadCore::TGenericHardwareInfo`. Replace the bodies with calls to your
// concrete Linux implementation in
// `Source/Private/Implementations/Platforms/Commons/CommonsDeviceInfo.cpp`
// (e.g., forward to your FCommonsDeviceInfo logic that uses SDL HID).
namespace FLinuxPlatform
{
	struct FLinuxHardwarePolicy;
	using FLinuxHardware = GamepadCore::TGenericHardwareInfo<FLinuxHardwarePolicy>;
	
    struct FLinuxHardwarePolicy
    {
        FLinuxHardwarePolicy() = default;

        void Read(FDeviceContext* Context)
        {
			FCommonsDeviceInfo::Read(Context);
        }

        void Write(FDeviceContext* Context)
        {
        	FCommonsDeviceInfo::Write(Context);
        }

        void Detect(std::vector<FDeviceContext>& Devices)
        {
        	FCommonsDeviceInfo::Detect(Devices);
        }

        bool CreateHandle(FDeviceContext* Context)
        {
        	return FCommonsDeviceInfo::CreateHandle(Context);
        }

        void InvalidateHandle(FDeviceContext* Context)
        {
        	FCommonsDeviceInfo::InvalidateHandle(Context);
        }

        void ProcessAudioHaptic(FDeviceContext* Context)
        {
        	FCommonsDeviceInfo::ProcessAudioHapitc(Context);
        }
    };
}
