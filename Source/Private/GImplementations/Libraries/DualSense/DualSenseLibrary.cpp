// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "GImplementations/Libraries/DualSense/DualSenseLibrary.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Types/ECoreGamepad.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "GImplementations/Utils/GamepadInput.h"
#include "GImplementations/Utils/GamepadOutput.h"
#include "GImplementations/Utils/GamepadSensors.h"
#include "GImplementations/Utils/GamepadTouch.h"
#include "GImplementations/Utils/GamepadTrigger.h"
#include <thread>

using namespace FDualSenseTriggerComposer;

void FDualSenseLibrary::SetVibration(std::uint8_t LeftRumble,
                                     std::uint8_t RightRumble)
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

void FDualSenseLibrary::ResetLights()
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context)
	{
		return;
	}

	FOutputContext* HidOutput = &Context->Output;
	if (HidOutput->Lightbar.G == 0 && HidOutput->Lightbar.B == 0 &&
	    HidOutput->Lightbar.R == 0)
	{
		HidOutput->Lightbar.B = 255;
	}

	HidOutput->PlayerLed.Led = static_cast<unsigned char>(EDSPlayer::One);
	UpdateOutput();
}

void FDualSenseLibrary::SetLightbar(DSCoreTypes::FDSColor Color)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context)
	{
		return;
	}

	FOutputContext* HidOutput = &Context->Output;
	if ((HidOutput->Lightbar.R != Color.R) ||
	    (HidOutput->Lightbar.G != Color.G) ||
	    (HidOutput->Lightbar.B != Color.B))
	{
		HidOutput->Lightbar.R = Color.R;
		HidOutput->Lightbar.G = Color.G;
		HidOutput->Lightbar.B = Color.B;
		UpdateOutput();
	}
}

bool FDualSenseLibrary::Initialize(const FDeviceContext& Context)
{
	SetDeviceContexts(Context);
	FDeviceContext* DSContext = GetMutableDeviceContext();
	if (DSContext->ConnectionType == EDSDeviceConnection::Bluetooth)
	{
		FOutputContext* EnableReport = &DSContext->Output;
		// Set flags to enable control over the lightbar, player LEDs
		EnableReport->Feature.FeatureMode = 0b11110111;
		EnableReport->Lightbar = {255, 255, 255};
		EnableReport->PlayerLed.Brightness = 0x00;
		EnableReport->PlayerLed.Led = static_cast<unsigned char>(EDSPlayer::One);
		UpdateOutput();

		// Audio haptics bluetooth
		DSContext->BufferAudio[0] = 0x32;
		DSContext->BufferAudio[1] = 0x00;
		DSContext->BufferAudio[2] = 0x91;
		DSContext->BufferAudio[3] = 0x07;
		DSContext->BufferAudio[4] = 0xFE;
		DSContext->BufferAudio[5] = 55;
		DSContext->BufferAudio[6] = 55;
		DSContext->BufferAudio[7] = 15;
		DSContext->BufferAudio[8] = 50;
		DSContext->BufferAudio[9] = 50;

		return true;
	}

	ResetLights();
	return true;
}

void FDualSenseLibrary::UpdateInput(float /*Delta*/)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context || !Context->IsConnected)
	{
		return;
	}

	IPlatformHardwareInfo::Get().Read(Context);
	FInputContext* InputToFill = Context->GetBackBuffer();
	const size_t Padding =
	    Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 2 : 1;

	using namespace FGamepadInput;
	DualSenseRaw(&Context->Buffer[Padding], InputToFill);

	if (Context->bEnableGesture || Context->bEnableTouch)
	{
		using namespace FGamepadTouch;
		ProcessTouch(&Context->Buffer[Padding], InputToFill);
	}

	if (Context->bEnableAccelerometerAndGyroscope)
	{
		DSCoreTypes::DSVector3D GyroDeg;
		DSCoreTypes::DSVector3D AccelG;

		using namespace FGamepadSensors;
		ProcessMotionData(&Context->Buffer[Padding], Context->Calibration, GyroDeg, AccelG);

		InputToFill->Gyroscope = GyroDeg;
		InputToFill->Accelerometer = AccelG;
	}

	static bool bLastMuteState = false;
	if (InputToFill->bMute && !bLastMuteState)
	{
		Context->Output.Audio.MicStatus = (Context->Output.Audio.MicStatus == 0) ? 1 : 0;
		UpdateOutput();
	}
	bLastMuteState = InputToFill->bMute;

	Context->SwapInputBuffers();
}

void FDualSenseLibrary::DualSenseSettings(std::uint8_t bIsMic, std::uint8_t bIsHeadset, std::uint8_t bIsSpeaker, std::uint8_t MicVolume, std::uint8_t AudioVolume, std::uint8_t RumbleMode, std::uint8_t RumbleReduce, std::uint8_t TriggerReduce)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Context->Output.Audio.MicStatus = bIsMic;
	Context->Output.Audio.MicVolume = MicVolume;
	Context->Output.Audio.HeadsetVolume = AudioVolume;
	Context->Output.Audio.SpeakerVolume = AudioVolume;

	Context->Output.Audio.Mode = 0x05;
	if (bIsHeadset == 1 && bIsSpeaker == 1)
	{
		Context->Output.Audio.Mode = 0x21;
	}
	else if (bIsHeadset == 0 && bIsSpeaker == 1)
	{
		Context->Output.Audio.Mode = 0x31;
	}

	Context->Output.Feature = {RumbleMode, RumbleReduce, TriggerReduce};
	UpdateOutput();
}

void FDualSenseLibrary::UpdateOutput()
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context || !Context->IsConnected)
	{
		return;
	}
	FGamepadOutput::OutputDualSense(Context);
}

void FDualSenseLibrary::SetResistance(std::uint8_t StartZones,
                                      std::uint8_t Strength,
                                      const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Resistance(Context, StartZones, Strength, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetGalloping23(std::uint8_t StartPosition,
                                       std::uint8_t EndPosition,
                                       std::uint8_t FirstFoot,
                                       std::uint8_t SecondFoot,
                                       std::uint8_t Frequency,
                                       const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Galloping23(Context, StartPosition, EndPosition, FirstFoot, SecondFoot,
	            Frequency, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::StopTrigger(const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Off(Context, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetGameCube(const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	GameCube(Context, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetBow22(std::uint8_t StartZone, std::uint8_t SnapBack,
                                 const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Context->bOverrideTriggerBytes = false;
	Bow22(Context, StartZone, SnapBack, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetWeapon25(std::uint8_t StartZone,
                                    std::uint8_t Amplitude,
                                    std::uint8_t Behavior, std::uint8_t Trigger,
                                    const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Weapon25(Context, StartZone, Amplitude, Behavior, Trigger, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetMachineGun26(std::uint8_t StartZone,
                                        std::uint8_t Behavior,
                                        std::uint8_t Amplitude,
                                        std::uint8_t Frequency,
                                        const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	MachineGun26(Context, StartZone, Behavior, Amplitude, Frequency, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetMachine27(std::uint8_t StartZone,
                                     std::uint8_t BehaviorFlag,
                                     std::uint8_t Force, std::uint8_t Amplitude,
                                     std::uint8_t Period,
                                     std::uint8_t Frequency,
                                     const EDSGamepadHand& Hand)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Machine27(Context, StartZone, BehaviorFlag, Force, Amplitude, Period,
	          Frequency, Hand);
	UpdateOutput();
}

void FDualSenseLibrary::SetCustomTrigger(
    const EDSGamepadHand& Hand, const std::vector<std::uint8_t>& HexBytes)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	CustomTrigger(Context, Hand, HexBytes);

	UpdateOutput();
}

void FDualSenseLibrary::SetPlayerLed(EDSPlayer Led, std::uint8_t Brightness)
{
	FOutputContext* HidOutput = &GetMutableDeviceContext()->Output;
	if ((HidOutput->PlayerLed.Led != static_cast<unsigned char>(Led)) || (HidOutput->PlayerLed.Brightness !=
	                                                                      static_cast<unsigned char>(Brightness)))
	{
		HidOutput->PlayerLed.Led = static_cast<unsigned char>(Led);
		HidOutput->PlayerLed.Brightness = static_cast<unsigned char>(Brightness);
		UpdateOutput();
	}
}

void FDualSenseLibrary::SetMicrophoneLed(EDSMic /*Led*/)
{
}

void FDualSenseLibrary::AudioHapticUpdate(std::vector<std::uint8_t> Data)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context || !Context->IsConnected)
	{
		return;
	}

	unsigned char* AudioData = &Context->BufferAudio[10];
	AudioData[0] = (AudioVibrationSequence++) & 0xFF;
	AudioData[1] = 0x92;
	AudioData[2] = 0x40;
	std::memcpy(&AudioData[3], Data.data(), 64);
	FGamepadOutput::SendAudioHapticAdvanced(Context);
}

void FDualSenseLibrary::AudioHapticUpdate(std::vector<std::vector<std::int16_t>> AudioData)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	if (!Context || !Context->AudioContext || !Context->AudioContext->IsValid())
	{
		return;
	}

	// Use the new miniaudio-based WriteHapticData method
	Context->AudioContext->WriteHapticData(AudioData);
}
