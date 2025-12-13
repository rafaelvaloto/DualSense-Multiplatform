// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "Implementations/Libraries/DualSense/DualSenseLibrary.h"
#include "Core/Algorithms/MadgwickAhrs.h"
#include "Core/Interfaces/IPlatformHardwareInfo.h"
#include "Core/Types/ECoreGamepad.h"
#include "Core/Types/Structs/Context/DeviceContext.h"
#include "Implementations/Utils/GamepadInput.h"
#include "Implementations/Utils/GamepadOutput.h"
#include "Implementations/Utils/GamepadSensors.h"
#include "Implementations/Utils/GamepadTouch.h"
#include "Implementations/Utils/GamepadTrigger.h"

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
	if (HidOutput->Lightbar.A == 0 && HidOutput->Lightbar.B == 0 &&
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
		EnableReport->Feature.FeatureMode = 0x55;
		EnableReport->Lightbar = {0, 0, 222};
		EnableReport->PlayerLed.Brightness = 0x00;
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
	}

	ResetLights();
	return true;
}

void FDualSenseLibrary::UpdateInput(float Delta)
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

	if (IsEnableTouch())
	{
		using namespace FGamepadTouch;
		ProcessTouch(&Context->Buffer[Padding], InputToFill);
	}

	if (IsEnableAccelerometerAndGyroscope())
	{
		DSCoreTypes::DSVector3D GyroDeg;
		DSCoreTypes::DSVector3D AccelG;

		using namespace FGamepadSensors;
		ProcessMotionData(&Context->Buffer[Padding], Context->Calibration, GyroDeg,
		                  AccelG);

		if (IsResetGyroscope())
		{
			MadgwickFilter.Reset();
		}
		constexpr float GToMSq = GRAVITY_MS2;
		constexpr float DegToRad = 3.1415926535f / 180.0f;
		const DSCoreTypes::DSVector3D AccelRad = {AccelG.X * GToMSq, AccelG.Y * GToMSq,
		                                          AccelG.Z * GToMSq};
		const DSCoreTypes::DSVector3D GyroRad = {GyroDeg.X * DegToRad, GyroDeg.Y * DegToRad,
		                                         GyroDeg.Z * DegToRad}; // deg/s -> rad/s
		MadgwickFilter.UpdateImu(GyroRad.Z, GyroRad.Y, -GyroRad.X, AccelRad.Z,
		                         AccelRad.Y, -AccelRad.X, Delta);

		float qw, qx, qy, qz;
		MadgwickFilter.GetQuaternion(qw, qx, qy, qz);

		constexpr DSCoreTypes::DSQuat RotX180(1.0f, 0.0f, 0.0f, 0.0f);
		const DSCoreTypes::DSQuat RawQuat(qx, qy, qz, qw);
		const DSCoreTypes::DSQuat SensorQuat = RotX180 * RawQuat;

		InputToFill->Gyroscope.X = GyroDeg.X;
		InputToFill->Gyroscope.Y = GyroDeg.Z;
		InputToFill->Gyroscope.Y = GyroDeg.Y;
		InputToFill->Accelerometer.X = AccelG.X;
		InputToFill->Accelerometer.Y = AccelG.Z;
		InputToFill->Accelerometer.Y = AccelG.Y;
		InputToFill->Gravity = RawQuat.GetUpVector();

		auto [Pitch, Yaw, Roll] = SensorQuat.ToRotator();
		InputToFill->Tilt.X = Roll;
		InputToFill->Tilt.Y = Yaw;
		InputToFill->Tilt.Z = Pitch;
	}

	Context->SwapInputBuffers();
}

void FDualSenseLibrary::DualSenseSettings(std::uint8_t bIsMic, std::uint8_t bIsHeadset, std::uint8_t bIsSpeaker, std::uint8_t MicVolume, std::uint8_t AudioVolume, std::uint8_t RumbleMode, std::uint8_t RumbleReduce, std::uint8_t TriggerReduce)
{
	FDeviceContext* Context = GetMutableDeviceContext();
	Context->Output.Audio.MicStatus = bIsMic;
	Context->Output.Audio.MicVolume = MicVolume;
	Context->Output.Audio.HeadsetVolume = AudioVolume;
	Context->Output.Audio.SpeakerVolume = AudioVolume;

	Context->Output.Audio.Mode = 0x08;
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
}

void FDualSenseLibrary::SetMicrophoneLed(EDSMic Led)
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
