// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

namespace FGamepadSensors
{
	inline void DualSenseCalibrationSensors(const std::uint8_t* Buffer,
	                                        FGamepadCalibration& OutCalibration)
	{
		auto GetLE16 = [](const std::uint8_t* Data) -> std::uint16_t {
			return static_cast<std::uint16_t>(Data[0] | (Data[1] << 8));
		};

		const std::uint16_t GyroPitchBias = GetLE16(&Buffer[1]);
		const std::uint16_t GyroYawBias = GetLE16(&Buffer[3]);
		const std::uint16_t GyroRollBias = GetLE16(&Buffer[5]);

		const std::uint16_t GyroPitchPlus = GetLE16(&Buffer[7]);
		const std::uint16_t GyroPitchMinus = GetLE16(&Buffer[9]);
		const std::uint16_t GyroYawPlus = GetLE16(&Buffer[11]);
		const std::uint16_t GyroYawMinus = GetLE16(&Buffer[13]);
		const std::uint16_t GyroRollPlus = GetLE16(&Buffer[15]);
		const std::uint16_t GyroRollMinus = GetLE16(&Buffer[17]);

		const std::uint16_t GyroSpeedPlus = GetLE16(&Buffer[19]);
		const std::uint16_t GyroSpeedMinus = GetLE16(&Buffer[21]);

		const std::uint16_t AccelXPlus = GetLE16(&Buffer[23]);
		const std::uint16_t AccelXMinus = GetLE16(&Buffer[25]);
		const std::uint16_t AccelYPlus = GetLE16(&Buffer[27]);
		const std::uint16_t AccelYMinus = GetLE16(&Buffer[29]);
		const std::uint16_t AccelZPlus = GetLE16(&Buffer[31]);
		const std::uint16_t AccelZMinus = GetLE16(&Buffer[33]);

		OutCalibration.GyroBiasX = GyroPitchBias;
		OutCalibration.GyroBiasY = GyroYawBias;
		OutCalibration.GyroBiasZ = GyroRollBias;

		const float Speed2x = static_cast<float>(GyroSpeedPlus + GyroSpeedMinus);
		float DenomX = static_cast<float>(std::abs(static_cast<int>(GyroPitchPlus) -
		                                           static_cast<int>(GyroPitchBias)) +
		                                  std::abs(static_cast<int>(GyroPitchMinus) -
		                                           static_cast<int>(GyroPitchBias)));
		OutCalibration.GyroFactorX = (DenomX != 0.0f) ? (Speed2x / DenomX) : 1.0f;

		float DenomY = static_cast<float>(
		    std::abs(static_cast<int>(GyroYawPlus) - static_cast<int>(GyroYawBias)) +
		    std::abs(static_cast<int>(GyroYawMinus) - static_cast<int>(GyroYawBias)));
		OutCalibration.GyroFactorY = (DenomY != 0.0f) ? (Speed2x / DenomY) : 1.0f;

		float DenomZ = static_cast<float>(std::abs(static_cast<int>(GyroRollPlus) -
		                                           static_cast<int>(GyroRollBias)) +
		                                  std::abs(static_cast<int>(GyroRollMinus) -
		                                           static_cast<int>(GyroRollBias)));
		OutCalibration.GyroFactorZ = (DenomZ != 0.0f) ? (Speed2x / DenomZ) : 1.0f;

		// Acc X
		const float RangeX = static_cast<float>(AccelXPlus - AccelXMinus);
		OutCalibration.AccelBiasX = (AccelXPlus + AccelXMinus) / 2.0f;
		OutCalibration.AccelFactorX = (RangeX != 0.0f) ? (2.0f / RangeX) : 1.0f;

		// Acc Y
		const float RangeY = static_cast<float>(AccelYPlus - AccelYMinus);
		OutCalibration.AccelBiasY = (AccelYPlus + AccelYMinus) / 2.0f;
		OutCalibration.AccelFactorY = (RangeY != 0.0f) ? (2.0f / RangeY) : 1.0f;

		// Acc Z
		const float RangeZ = static_cast<float>(AccelZPlus - AccelZMinus);
		OutCalibration.AccelBiasZ = (AccelZPlus + AccelZMinus) / 2.0f;
		OutCalibration.AccelFactorZ = (RangeZ != 0.0f) ? (2.0f / RangeZ) : 1.0f;
	}

	inline void ProcessMotionData(const std::uint8_t* Buffer,
	                              const FGamepadCalibration& Calibration,
	                              DSCoreTypes::DSVector3D& FinalGyro,
	                              DSCoreTypes::DSVector3D& FinalAccel)
	{
		const std::int16_t RawGyroX = (Buffer[15] | (Buffer[16] << 8));
		const std::int16_t RawGyroY = (Buffer[17] | (Buffer[18] << 8));
		const std::int16_t RawGyroZ = (Buffer[19] | (Buffer[20] << 8));

		const std::uint16_t RawAccX = (Buffer[21] | (Buffer[22] << 8));
		const std::uint16_t RawAccY = (Buffer[23] | (Buffer[24] << 8));
		const std::uint16_t RawAccZ = (Buffer[25] | (Buffer[26] << 8));

		FinalGyro.X = ((RawGyroX - Calibration.GyroBiasX) * Calibration.GyroFactorX);
		FinalGyro.Y = ((RawGyroY - Calibration.GyroBiasY) * Calibration.GyroFactorY);
		FinalGyro.Z = ((RawGyroZ - Calibration.GyroBiasZ) * Calibration.GyroFactorZ);

		FinalAccel.X =
		    ((RawAccX - Calibration.AccelBiasX) * Calibration.AccelFactorX);
		FinalAccel.Y =
		    ((RawAccY - Calibration.AccelBiasY) * Calibration.AccelFactorY);
		FinalAccel.Z =
		    ((RawAccZ - Calibration.AccelBiasZ) * Calibration.AccelFactorZ);
	}

} // namespace FGamepadSensors
