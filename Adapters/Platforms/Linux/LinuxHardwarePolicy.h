// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

#include "Core/Templates/TGenericHardwareInfo.h"
#include "Core/Interfaces/IPlatformHardwareInfo.h"

// Sample Linux hardware policy adapter template
//
// This example satisfies the `IsHardwarePolicy` concept used by
// `GamepadCore::TGenericHardwareInfo`. Replace the bodies with calls to your
// concrete Linux implementation in
// `Source/Private/Implementations/Platforms/Commons/CommonsDeviceInfo.cpp`
// (e.g., forward to your FCommonsDeviceInfo logic that uses SDL HID).

namespace GamepadCore::Adapters::Platforms::Linux
{
    struct LinuxHardwarePolicy
    {
        LinuxHardwarePolicy() = default;

        void Read(FDeviceContext* Context)
        {
#if defined(__unix__)
            // TODO: Forward to your Linux implementation
            // e.g., FLinuxDeviceInfo::Read(Context);
            (void)Context;
#else
            (void)Context;
#endif
        }

        void Write(FDeviceContext* Context)
        {
#if defined(__unix__)
            // TODO: Forward to your Linux implementation
            (void)Context;
#else
            (void)Context;
#endif
        }

        void Detect(std::vector<FDeviceContext>& Devices)
        {
#if defined(__unix__)
            // TODO: Forward to your Linux implementation
            (void)Devices;
#else
            (void)Devices;
#endif
        }

        bool CreateHandle(FDeviceContext* Context)
        {
#if defined(__unix__)
            // TODO: Forward to your Linux implementation
            (void)Context;
            return false;
#else
            (void)Context;
            return false;
#endif
        }

        void InvalidateHandle(FDeviceContext* Context)
        {
#if defined(__unix__)
            // TODO: Forward to your Linux implementation
            (void)Context;
#else
            (void)Context;
#endif
        }

        void ProcessAudioHaptic(FDeviceContext* Context)
        {
#if defined(__unix__)
            // TODO: Forward to your Linux implementation
            (void)Context;
#else
            (void)Context;
#endif
        }
    };

    // Example alias showing how to plug the policy into the generic adapter
    using LinuxHardware = GamepadCore::TGenericHardwareInfo<LinuxHardwarePolicy>;
}
