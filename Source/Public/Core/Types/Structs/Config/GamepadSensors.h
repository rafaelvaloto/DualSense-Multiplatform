// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

/**
 * @brief Represents a structure for calibrating gamepad sensors.
 *
 * This structure is used to store bias and normalization parameters for
 * gyroscopic and accelerometer data retrieved from a gamepad. The values stored
 * in this structure are utilized to adjust raw sensor data for more accurate
 * readings.
 *
 * Attributes:
 * - GyroBiasX, GyroBiasY, GyroBiasZ: Bias offsets for the gyroscope on the X,
 * Y, and Z axes.
 * - AccelBiasX, AccelBiasY, AccelBiasZ: Bias offsets for the accelerometer on
 * the X, Y, and Z axes.
 * - GyroFactorX, GyroFactorY, GyroFactorZ: Normalization factors for the
 * gyroscope on the X, Y, and Z axes.
 * - AccelFactorX, AccelFactorY, AccelFactorZ: Normalization factors for the
 * accelerometer on the X, Y, and Z axes.
 */
struct FGamepadCalibration
{
	std::uint16_t GyroBiasX = 0;
	std::uint16_t GyroBiasY = 0;
	std::uint16_t GyroBiasZ = 0;

	float AccelBiasX = 0;
	float AccelBiasY = 0;
	float AccelBiasZ = 0;

	float GyroFactorX = 1.0f;
	float GyroFactorY = 1.0f;
	float GyroFactorZ = 1.0f;

	float AccelFactorX = 1.0f;
	float AccelFactorY = 1.0f;
	float AccelFactorZ = 1.0f;
};
