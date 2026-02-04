// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "GImplementations/Utils/GamepadOutput.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/ECoreGamepad.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "GCore/Utils/CR32.h"
#include "GCore/Utils/SoDefines.h"
#include <iostream>
#include <ostream>

void FGamepadOutput::OutputDualShock(FDeviceContext* DeviceContext)
{
	const FOutputContext* HidOut = &DeviceContext->Output;
	unsigned char* MutableBuffer = DeviceContext->GetRawOutputBuffer();

	// Reset buffer
	std::memset(MutableBuffer, 0, 78);

	if (DeviceContext->ConnectionType == EDSDeviceConnection::Bluetooth)
	{
		// Bluetooth: Report ID 0x11, Data starts at offset 3
		MutableBuffer[0] = 0x11;
		MutableBuffer[1] = 0x80; // Report sub-ID
		MutableBuffer[3] = 0x0F; // Control mask: Rumble, Lightbar, Flash

		MutableBuffer[6] = HidOut->Rumbles.Left;
		MutableBuffer[7] = HidOut->Rumbles.Right;
		MutableBuffer[8] = HidOut->Lightbar.R;
		MutableBuffer[9] = HidOut->Lightbar.G;
		MutableBuffer[10] = HidOut->Lightbar.B;
		MutableBuffer[11] = HidOut->FlashLigthbar.Bright_Time;
		MutableBuffer[12] = HidOut->FlashLigthbar.Toggle_Time;

		const auto CrcChecksum = GCoreUtils::CR32::Compute(MutableBuffer, 74);
		MutableBuffer[74] = static_cast<unsigned char>((CrcChecksum & 0x000000FF) >> 0UL);
		MutableBuffer[75] = static_cast<unsigned char>((CrcChecksum & 0x0000FF00) >> 8UL);
		MutableBuffer[76] = static_cast<unsigned char>((CrcChecksum & 0x00FF0000) >> 16UL);
		MutableBuffer[77] = static_cast<unsigned char>((CrcChecksum & 0xFF000000) >> 24UL);
	}
	else
	{
		// USB: Report ID 0x05, Data starts at offset 1
		MutableBuffer[0] = 0x05;
		MutableBuffer[1] = 0x0F; // Control mask: Rumble, Lightbar, Flash

		MutableBuffer[4] = HidOut->Rumbles.Left;
		MutableBuffer[5] = HidOut->Rumbles.Right;
		MutableBuffer[6] = HidOut->Lightbar.R;
		MutableBuffer[7] = HidOut->Lightbar.G;
		MutableBuffer[8] = HidOut->Lightbar.B;
		MutableBuffer[9] = HidOut->FlashLigthbar.Bright_Time;
		MutableBuffer[10] = HidOut->FlashLigthbar.Toggle_Time;
	}

	{
		gc_lock::lock_guard<gc_lock::mutex> LockGuard(DeviceContext->OutputMutex);
		IPlatformHardwareInfo::Get().Write(DeviceContext);
	}
}

void FGamepadOutput::OutputDualSense(FDeviceContext* DeviceContext)
{
	FOutputContext* HidOut = &DeviceContext->Output;
	size_t Padding = 1;
	unsigned char* MutableBuffer = DeviceContext->GetRawOutputBuffer();
	MutableBuffer[0] = 0x02;
	if (DeviceContext->ConnectionType == EDSDeviceConnection::Bluetooth)
	{
		Padding = 2;
		MutableBuffer[0] = 0x31;
		MutableBuffer[1] = 0x02;
		MutableBuffer[40] ^= 0x01;
	}
	else
	{
		MutableBuffer[40] = 0x07;
	}

	{
		gc_lock::lock_guard<gc_lock::mutex> LockGuard(DeviceContext->OutputMutex);

		unsigned char* Output = &MutableBuffer[Padding];
		Output[0] = HidOut->Feature.VibrationMode;
		Output[1] = HidOut->Feature.FeatureMode;
		Output[2] = HidOut->Rumbles.Left;
		Output[3] = HidOut->Rumbles.Right;
		Output[4] = HidOut->Audio.HeadsetVolume;
		Output[5] = HidOut->Audio.SpeakerVolume;
		Output[6] = HidOut->Audio.MicVolume;
		Output[7] = HidOut->Audio.Mode;
		Output[9] = HidOut->Audio.MicStatus == 1 ? 0x10 : 0x00;
		Output[8] = HidOut->Audio.MicStatus == 1 ? 0x01 : 0x00;
		Output[36] = (HidOut->Feature.TriggerSoftnessLevel << 4) | (HidOut->Feature.SoftRumbleReduce & 0x0F);
		Output[42] = HidOut->PlayerLed.Brightness;
		Output[43] = HidOut->PlayerLed.Led;
		Output[44] = HidOut->Lightbar.R;
		Output[45] = HidOut->Lightbar.G;
		Output[46] = HidOut->Lightbar.B;

		if (DeviceContext->bOverrideTriggerBytes)
		{
			std::memcpy(&Output[10], DeviceContext->OverrideTriggerRight, 10);
			std::memcpy(&Output[21], DeviceContext->OverrideTriggerLeft, 10);
		}
		else
		{
			SetTriggerEffects(&Output[10], HidOut->RightTrigger);
			SetTriggerEffects(&Output[21], HidOut->LeftTrigger);
		}

		if (DeviceContext->ConnectionType == EDSDeviceConnection::Bluetooth)
		{
			const auto CrcChecksum = GCoreUtils::CR32::Compute(MutableBuffer, 74);
			MutableBuffer[0x4A] = static_cast<unsigned char>((CrcChecksum & 0x000000FF) >> 0UL);
			MutableBuffer[0x4B] = static_cast<unsigned char>((CrcChecksum & 0x0000FF00) >> 8UL);
			MutableBuffer[0x4C] = static_cast<unsigned char>((CrcChecksum & 0x00FF0000) >> 16UL);
			MutableBuffer[0x4D] = static_cast<unsigned char>((CrcChecksum & 0xFF000000) >> 24UL);
		}

		IPlatformHardwareInfo::Get().Write(DeviceContext);
	}
}

void FGamepadOutput::SetTriggerEffects(unsigned char* Trigger, FGamepadTriggersHaptic& Effect)
{
	Trigger[0x0] = Effect.Mode;

	if (Effect.Mode == 0x01) // Continuous Resistance
	{
		Trigger[0x1] = Effect.Strengths.Compose[0];
		Trigger[0x2] = Effect.Strengths.Compose[1];
	}

	if (Effect.Mode == 0x21) // Resistance
	{
		Trigger[0x1] = Effect.Strengths.Compose[0];
		Trigger[0x2] = Effect.Strengths.Compose[1];
		Trigger[0x3] = Effect.Strengths.Compose[2];
		Trigger[0x5] = Effect.Strengths.Compose[3];
		Trigger[0x6] = Effect.Strengths.Compose[4];
		Trigger[0x7] = 0x0;
		Trigger[0x8] = 0x0;
		Trigger[0x9] = 0x0;
	}

	if (Effect.Mode == 0x22 || Effect.Mode == 0x02) // Bow
	{
		Trigger[0x1] = Effect.Strengths.Compose[0];
		Trigger[0x2] = Effect.Strengths.Compose[1];
		Trigger[0x3] = Effect.Strengths.Compose[2];
		Trigger[0x4] = Effect.Strengths.Compose[3];
		Trigger[0x5] = Effect.Strengths.Compose[4];
		Trigger[0x6] = 0x0;
		Trigger[0x7] = 0x0;
		Trigger[0x8] = 0x0;
		Trigger[0x9] = 0x0;
	}

	if (Effect.Mode == 0x23) // Gallopping
	{
		Trigger[0x1] = Effect.Strengths.Compose[0];
		Trigger[0x2] = Effect.Strengths.Compose[1];
		Trigger[0x3] = Effect.Strengths.Compose[2];
		Trigger[0x4] = Effect.Strengths.Compose[3];
		Trigger[0x5] = 0x00;
		Trigger[0x6] = 0x00;
		Trigger[0x7] = 0x00;
		Trigger[0x8] = 0x00;
		Trigger[0x9] = 0x00;
	}

	if (Effect.Mode == 0x25) // Weapon
	{
		Trigger[0x1] = Effect.Strengths.Compose[0];
		Trigger[0x2] = Effect.Strengths.Compose[1];
		Trigger[0x3] = Effect.Strengths.Compose[2];
		Trigger[0x4] = 0x00;
		Trigger[0x5] = 0x00;
		Trigger[0x6] = 0x00;
		Trigger[0x7] = 0x00;
		Trigger[0x8] = 0x00;
		Trigger[0x9] = 0x00;
	}

	if (Effect.Mode == 0x26) // Automatic Gun
	{
		Trigger[0x1] = Effect.Strengths.Compose[0];
		Trigger[0x2] = Effect.Strengths.Compose[1];
		Trigger[0x3] = Effect.Strengths.Compose[2];
		Trigger[0x4] = Effect.Strengths.Compose[3];
		Trigger[0x5] = Effect.Strengths.Compose[4];
		Trigger[0x6] = Effect.Strengths.Compose[5];
		Trigger[0x7] = 0x0;
		Trigger[0x8] = 0x0;
		Trigger[0x9] = Effect.Strengths.Compose[9];
	}

	if (Effect.Mode == 0x27) // Machine Advanced
	{
		// Structure: [27] [Start_Zone] [Behavior_Flag] [Force_Amplitude] [Period]
		// [Frequency]
		Trigger[0x1] = Effect.Strengths.Compose[0]; // Start_Zone
		Trigger[0x2] = Effect.Strengths.Compose[1]; // Behavior_Flag
		Trigger[0x3] = Effect.Strengths.Compose[2]; // Force_Amplitude
		Trigger[0x4] = Effect.Strengths.Compose[3]; // Period
		Trigger[0x5] = Effect.Strengths.Compose[4]; // Frequency
		Trigger[0x6] = 0x00;
		Trigger[0x7] = 0x00;
		Trigger[0x8] = 0x00;
		Trigger[0x9] = 0x00;
	}

	if (Effect.Mode == 0xFF) // Custom Mode effect
	{
		Trigger[0x0] = Effect.Strengths.Compose[0];
		Trigger[0x1] = Effect.Strengths.Compose[1];
		Trigger[0x2] = Effect.Strengths.Compose[2];
		Trigger[0x3] = Effect.Strengths.Compose[3];
		Trigger[0x4] = Effect.Strengths.Compose[4];
		Trigger[0x5] = Effect.Strengths.Compose[5];
		Trigger[0x6] = Effect.Strengths.Compose[6];
		Trigger[0x7] = Effect.Strengths.Compose[7];
		Trigger[0x8] = Effect.Strengths.Compose[8];
		Trigger[0x9] = Effect.Strengths.Compose[9];
	}

	if (Effect.Mode == 0x0) // Reset
	{
		Trigger[0x1] = 0x0;
		Trigger[0x2] = 0x0;
		Trigger[0x3] = 0x0;
		Trigger[0x4] = 0x0;
		Trigger[0x5] = 0x0;
		Trigger[0x6] = 0x0;
		Trigger[0x7] = 0x0;
		Trigger[0x8] = 0x0;
		Trigger[0x9] = 0x0;
	}
}

void FGamepadOutput::SendAudioHapticAdvanced(
    FDeviceContext* DeviceContext)
{
	if (!DeviceContext)
	{
		return;
	}

	if (DeviceContext->ConnectionType == EDSDeviceConnection::Bluetooth)
	{
		constexpr size_t CrcOffset = 138;
		const auto CrcChecksum = GCoreUtils::CR32::Compute(DeviceContext->BufferAudio, CrcOffset);
		DeviceContext->BufferAudio[CrcOffset + 0] = static_cast<unsigned char>((CrcChecksum & 0x000000FF) >> 0UL);
		DeviceContext->BufferAudio[CrcOffset + 1] = static_cast<unsigned char>((CrcChecksum & 0x0000FF00) >> 8UL);
		DeviceContext->BufferAudio[CrcOffset + 2] = static_cast<unsigned char>((CrcChecksum & 0x00FF0000) >> 16UL);
		DeviceContext->BufferAudio[CrcOffset + 3] = static_cast<unsigned char>((CrcChecksum & 0xFF000000) >> 24UL);

		IPlatformHardwareInfo::Get().ProcessAudioHaptic(DeviceContext);
	}
}
