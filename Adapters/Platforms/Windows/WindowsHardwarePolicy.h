// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

#include "Core/Templates/TGenericHardwareInfo.h"
#include "Core/Interfaces/IPlatformHardwareInfo.h"

// Sample Windows hardware policy adapter template
//
// This is an example of a policy that satisfies the `IsHardwarePolicy` concept
// used by `GamepadCore::TGenericHardwareInfo`. Replace the bodies with calls
// to your concrete Windows implementation in
// `Source/Private/Implementations/Platforms/Windows/WindowsDeviceInfo.cpp`
// (e.g., forward to your FHIDDeviceInfo/FWindowsDeviceInfo logic).

namespace GamepadCore::Adapters::Platforms::Windows
{
    struct WindowsHardwarePolicy
    {
        WindowsHardwarePolicy() = default;

        void Read(FDeviceContext* Context)
        {
#if defined(_WIN32)
            // TODO: Forward to your Windows implementation
            // e.g., FWindowsDeviceInfo::Read(Context);
            (void)Context;
#else
            (void)Context;
#endif
        }

        void Write(FDeviceContext* Context)
        {
#if defined(_WIN32)
            // TODO: Forward to your Windows implementation
            (void)Context;
#else
            (void)Context;
#endif
        }

        void Detect(std::vector<FDeviceContext>& Devices)
        {
#if defined(_WIN32)
            // TODO: Forward to your Windows implementation
            (void)Devices;
#else
            (void)Devices;
#endif
        }

        bool CreateHandle(FDeviceContext* Context)
        {
#if defined(_WIN32)
            // TODO: Forward to your Windows implementation
            (void)Context;
            return false;
#else
            (void)Context;
            return false;
#endif
        }

        void InvalidateHandle(FDeviceContext* Context)
        {
#if defined(_WIN32)
            // TODO: Forward to your Windows implementation
            (void)Context;
#else
            (void)Context;
#endif
        }

        void ProcessAudioHaptic(FDeviceContext* Context)
        {
#if defined(_WIN32)
            // TODO: Forward to your Windows implementation
            (void)Context;
#else
            (void)Context;
#endif
        }
    };

    // Example alias showing how to plug the policy into the generic adapter
    using WindowsHardware = GamepadCore::TGenericHardwareInfo<WindowsHardwarePolicy>;
}
