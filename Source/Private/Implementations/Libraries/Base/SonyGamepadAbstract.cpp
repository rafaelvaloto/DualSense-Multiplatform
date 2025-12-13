// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "Implementations/Libraries/Base/SonyGamepadAbstract.h"
#include "Core/Interfaces/IPlatformHardwareInfo.h"

void SonyGamepadAbstract::ShutdownLibrary()
{
	IPlatformHardwareInfo::Get().InvalidateHandle(&HIDDeviceContexts);
}

void SonyGamepadAbstract::EnableTouch(const bool bIsTouch)
{
	bEnableTouch = bIsTouch;
}

void SonyGamepadAbstract::EnableGesture(const bool bIsTouch)
{
	bEnableGesture = bIsTouch;
}

void SonyGamepadAbstract::ResetGyroOrientation()
{
	bIsResetGyroscope = true;
}

void SonyGamepadAbstract::EnableMotionSensor(bool bIsMotionSensor)
{
	bEnableAccelerometerAndGyroscope = bIsMotionSensor;
}

float SonyGamepadAbstract::GetBattery()
{
	return HIDDeviceContexts.GetInputState().BatteryLevel;
}

bool SonyGamepadAbstract::IsConnected()
{
	return HIDDeviceContexts.IsConnected;
}

EDSDeviceType SonyGamepadAbstract::GetDeviceType()
{
	return HIDDeviceContexts.DeviceType;
}

EDSDeviceConnection SonyGamepadAbstract::GetConnectionType()
{
	return HIDDeviceContexts.ConnectionType;
}
