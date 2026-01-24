// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "GCore/Types/DSCoreTypes.h"
#include "GCore/Types/Structs/Config/GamepadCalibration.h"

namespace FGamepadSensors
{
	inline void DualShockCalibrationSensors(const std::uint8_t* /*Buffer*/,
	                                        FGamepadCalibration& /*OutCalibration*/)
	{
		// DualShock calibration implementation can be added here if needed
	}

	inline void DualSenseCalibrationSensors(const std::uint8_t* Buffer,
	                                        FGamepadCalibration& OutCalibration)
	{
		auto GetLE16 = [](const std::uint8_t* Data) -> std::int16_t {
			return static_cast<std::int16_t>(Data[0] | (Data[1] << 8));
		};

		// All are now int16_t
		const std::int16_t GyroPitchBias = GetLE16(&Buffer[1]);
		const std::int16_t GyroYawBias = GetLE16(&Buffer[3]);
		const std::int16_t GyroRollBias = GetLE16(&Buffer[5]);

		const std::int16_t GyroPitchPlus = GetLE16(&Buffer[7]);
		const std::int16_t GyroPitchMinus = GetLE16(&Buffer[9]);
		const std::int16_t GyroYawPlus = GetLE16(&Buffer[11]);
		const std::int16_t GyroYawMinus = GetLE16(&Buffer[13]);
		const std::int16_t GyroRollPlus = GetLE16(&Buffer[15]);
		const std::int16_t GyroRollMinus = GetLE16(&Buffer[17]);

		const std::int16_t GyroSpeedPlus = GetLE16(&Buffer[19]);
		const std::int16_t GyroSpeedMinus = GetLE16(&Buffer[21]);

		const std::int16_t AccelXPlus = GetLE16(&Buffer[23]);
		const std::int16_t AccelXMinus = GetLE16(&Buffer[25]);
		const std::int16_t AccelYPlus = GetLE16(&Buffer[27]);
		const std::int16_t AccelYMinus = GetLE16(&Buffer[29]);
		const std::int16_t AccelZPlus = GetLE16(&Buffer[31]);
		const std::int16_t AccelZMinus = GetLE16(&Buffer[33]);

		// Gyro Biases (Cleaned up duplicate code)
		OutCalibration.GyroBiasX = static_cast<float>(GyroPitchBias);
		OutCalibration.GyroBiasY = static_cast<float>(GyroYawBias);
		OutCalibration.GyroBiasZ = static_cast<float>(GyroRollBias);

		// Gyro Factors
		// Using float to ensure precision in sum before division
		const float Speed2x = static_cast<float>(GyroSpeedPlus + GyroSpeedMinus);

		// ABS logic works better now that they are int16
		float DenomX = static_cast<float>(std::abs(GyroPitchPlus - GyroPitchBias) + std::abs(GyroPitchMinus - GyroPitchBias));
		OutCalibration.GyroFactorX = (DenomX != 0.0f) ? (Speed2x / DenomX) : 1.0f;

		float DenomY = static_cast<float>(std::abs(GyroYawPlus - GyroYawBias) + std::abs(GyroYawMinus - GyroYawBias));
		OutCalibration.GyroFactorY = (DenomY != 0.0f) ? (Speed2x / DenomY) : 1.0f;

		float DenomZ = static_cast<float>(std::abs(GyroRollPlus - GyroRollBias) + std::abs(GyroRollMinus - GyroRollBias));
		OutCalibration.GyroFactorZ = (DenomZ != 0.0f) ? (Speed2x / DenomZ) : 1.0f;

		// Acc X
		// IMPORTANT: Now (XPlus - XMinus) will do the correct calculation: e.g.: 8192 - (-8192) = 16384
		const float RangeX = static_cast<float>(AccelXPlus - AccelXMinus);
		OutCalibration.AccelBiasX = (AccelXPlus + AccelXMinus) / 2.0f; // Bias will now be close to 0
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

	inline void ProcessMotionDualSense(const std::uint8_t* Buffer,
	                                   const FGamepadCalibration& Calibration,
	                                   DSCoreTypes::DSVector3D& FinalGyro,
	                                   DSCoreTypes::DSVector3D& FinalAccel)
	{
		const std::int16_t RawGyroX = static_cast<std::int16_t>(Buffer[15] | (Buffer[16] << 8));
		const std::int16_t RawGyroY = static_cast<std::int16_t>(Buffer[17] | (Buffer[18] << 8));
		const std::int16_t RawGyroZ = static_cast<std::int16_t>(Buffer[19] | (Buffer[20] << 8));

		const std::int16_t RawAccX = static_cast<std::int16_t>(Buffer[21] | (Buffer[22] << 8));
		const std::int16_t RawAccY = static_cast<std::int16_t>(Buffer[23] | (Buffer[24] << 8));
		const std::int16_t RawAccZ = static_cast<std::int16_t>(Buffer[25] | (Buffer[26] << 8));

		float fRawGyroX = static_cast<float>(RawGyroX);
		float fRawGyroY = static_cast<float>(RawGyroY);
		float fRawGyroZ = static_cast<float>(RawGyroZ);

		float fRawAccX = static_cast<float>(RawAccX);
		float fRawAccY = static_cast<float>(RawAccY);
		float fRawAccZ = static_cast<float>(RawAccZ);

		FinalGyro.X = (fRawGyroX - Calibration.GyroBiasX) * Calibration.GyroFactorX;
		FinalGyro.Y = (fRawGyroY - Calibration.GyroBiasY) * Calibration.GyroFactorY;
		FinalGyro.Z = (fRawGyroZ - Calibration.GyroBiasZ) * Calibration.GyroFactorZ;

		FinalAccel.X = (fRawAccX - Calibration.AccelBiasX) * Calibration.AccelFactorX;
		FinalAccel.Y = (fRawAccY - Calibration.AccelBiasY) * Calibration.AccelFactorY;
		FinalAccel.Z = (fRawAccZ - Calibration.AccelBiasZ) * Calibration.AccelFactorZ;
	}

	inline void ProcessMotionDualShock(const std::uint8_t* /*Buffer*/,
	                                   const FGamepadCalibration& /*Calibration*/,
	                                   DSCoreTypes::DSVector3D& /*FinalGyro*/,
	                                   DSCoreTypes::DSVector3D& /*FinalAccel*/)
	{
		// DualShock Motion processing can be implemented here if needed
	}

} // namespace FGamepadSensors
