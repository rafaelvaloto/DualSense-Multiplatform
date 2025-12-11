// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "Implementations/Libraries/DualShock/DualShockLibrary.h"
#include "Core/Interfaces/IPlatformHardwareInfo.h"
#include "Core/Types/ECoreGamepad.h"
#include "Implementations/Utils/GamepadInput.h"
#include "Implementations/Utils/GamepadOutput.h"

bool FDualShockLibrary::Initialize(const FDeviceContext& Context)
{
	SetDeviceContexts(Context);
	SetLightbar({0, 0, 220, 0}, 0.0f, 0.0f);
	return true;
}

void FDualShockLibrary::UpdateOutput()
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context->IsConnected)
	{
		return;
	}

	FGamepadOutput::OutputDualShock(Context);
}

void FDualShockLibrary::UpdateInput(float Delta)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	IPlatformHardwareInfo::Get().Read(Context);
	FInputContext* InputToFill = Context->GetBackBuffer();

	using namespace FGamepadInput;
	if (Context->ConnectionType == EDSDeviceConnection::Bluetooth)
	{
		DualShockRaw(&Context->BufferDS4[3], InputToFill);
	}
	else
	{
		DualShockRaw(&Context->Buffer[1], InputToFill);
	}
}

void FDualShockLibrary::SetVibration(std::uint8_t LeftRumble, std::uint8_t RightRumble)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context)
	{
		return;
	}

	FOutputContext* HidOutput = &Context->Output;
	if (HidOutput->Rumbles.Left != LeftRumble ||
	    HidOutput->Rumbles.Right != RightRumble)
	{
		HidOutput->Rumbles = {LeftRumble, RightRumble};
		UpdateOutput();
	}
}

void FDualShockLibrary::SetLightbar(FDSColor Color, float BrithnessTime, float ToggleTime)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	FOutputContext* HidOutput = &Context->Output;
	HidOutput->Lightbar.R = Color.R;
	HidOutput->Lightbar.G = Color.G;
	HidOutput->Lightbar.B = Color.B;

	HidOutput->FlashLigthbar.Bright_Time = static_cast<std::uint8_t>((BrithnessTime / 2.5f) * 255);
	HidOutput->FlashLigthbar.Toggle_Time = static_cast<std::uint8_t>((ToggleTime / 2.5f) * 255);
	UpdateOutput();
}

void FDualShockLibrary::ResetLights()
{
	SetLightbar({0, 0, 255, 0}, 0.0f, 0.0f);
}
